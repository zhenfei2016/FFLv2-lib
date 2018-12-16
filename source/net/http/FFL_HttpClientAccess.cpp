/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpClientAccess.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/15
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*/

#include <net/http/FFL_HttpClientAccess.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpResponseBuilder.hpp>
#include <net/http/FFL_HttpParser.hpp>
#include <net/http/FFL_HttpClient.hpp>

namespace FFL {	

	//
	//  请求，连接发送线程
	//
	class HttpRequestThread : public  FFL::Thread {
	public:
		HttpRequestThread(HttpClientAccessManager* client):mClient(client){}
		bool threadLoop() {
			int32_t errorNo = 0;
			FFL::sp<HttpClientAccessManager::RequestContext> entry = mClient->mRequestQueue.next(&errorNo);
			if (errorNo != 0) {
				//
				//  退出请求线程
				//
				return false;
			}			

			if (!entry.isEmpty()) {
				mClient->processRequest(entry);
			}
			return true;
		}
		HttpClientAccessManager* mClient;
	};


	HttpClientAccessManager::HttpClientAccessManager() :
		  mEventLoop(30 * 1000 * 1000),
		  mRequestQueue("httpRequestQueue") {		
	}
	HttpClientAccessManager::~HttpClientAccessManager() {

		FFL::CMutex::Autolock l(mPendingLock);
		mPendingRequstList.clear();
	}
	//
	//  启动，停止
	//
	bool HttpClientAccessManager::start() {
		if (!mHttpRequestThread.isEmpty()) { 
			FFL_LOG_DEBUG("HttpClientAccessManager: failed to start.");
			return false;
		}

		if (!mEventLoop.start(new ModuleThread("HttpClientAccessManager-eventloop"))) {
			FFL_LOG_DEBUG("HttpClientAccessManager: failed to start eventloop .");
			return false;
		}

		mHttpRequestThread = new HttpRequestThread(this);
		mHttpRequestThread->run("HttpClientAccessManager-request");
		mRequestQueue.start();
		return true;
	}
	void HttpClientAccessManager::stop() {
		mRequestQueue.stop();
		mEventLoop.stop();		
		if (!mHttpRequestThread.isEmpty()) {
			mHttpRequestThread->requestExitAndWait();
		}
		mRequestQueue.clear();
	}
	
	//
	//  发送一个请求
	//
	bool HttpClientAccessManager::post(FFL::sp<HttpRequest>& request, FFL::sp<Callback> callback) {
		if (request.isEmpty()) {
			return false;
		}

		FFL::sp<RequestContext> entry = new RequestContext();
		entry->mRequest = request;
		entry->mCallback = callback;
		entry->mCreateTimeUs = FFL_getNowUs();
		if (!mRequestQueue.incoming(entry)) {
			return false;
		}
		return true;
	}
	/*  NetEventLoop::Callback  */
	//
	//  返回是否还继续读写
	//  priv:透传数据
	//
	bool HttpClientAccessManager::onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv) {
		//
		//  找到请求
		//
		HttpClient* client = (HttpClient*)priv;
		FFL::sp<RequestContext> context;
		{
			FFL::CMutex::Autolock l(mPendingLock);
			for (std::list<  FFL::sp<RequestContext> >::iterator it = mPendingRequstList.begin();
				it != mPendingRequstList.end(); it++) {
				FFL::sp<RequestContext>& e = *it;
				if (e->mHttpClient.get() == client) {
					context = e;
					mPendingRequstList.erase(it);
					break;
				}
			}						
		}
		
		if (context.isEmpty()) {
			return false;
		}
		
        //
		// 读应答
		//
		FFL::sp<HttpResponse> response = context->mHttpClient->readResponse();
		FFL::sp<Callback> callback = context->mCallback;
		if (!callback.isEmpty()) {
			if (response.isEmpty()) {
				callback->onResponse(NULL, Callback::ERROR_UNKONW);
			}
			else {
				callback->onResponse(response, Callback::ERROR_SUC);
			}
		}	
		return false;
	}

	//
	//  处理这个请求
	//
	bool HttpClientAccessManager::processRequest(FFL::sp<RequestContext> entry) {	
		FFL::HttpUrl url;
		entry->mRequest->getUrl(url);
		
		TcpClient* client = new TcpClient();
		if (client->connect(url.mHost.c_str(), url.mPort, *client) == FFL_OK) {			
			NetFD fd = client->getFd();		
			FFL::sp<HttpClient> httpClient = new HttpClient(client);
			if (mEventLoop.addFd(fd, this, NULL, httpClient.get())) {
				entry->mRequest->setHttpClient(httpClient);
				if (entry->mRequest->writeHeader()) {
					//
					// 添加到等待应答的队列
					//
					{
						FFL::CMutex::Autolock l(mPendingLock);
						entry->mTcpClient = client;
						entry->mHttpClient = httpClient;
						mPendingRequstList.push_back(entry);
					}
					return true;
				} 			
				mEventLoop.removeFd(fd);
			}			
		}

		entry->mCallback->onResponse(NULL, Callback::ERROR_CONNECT);
		FFL_SafeFree(client);
		return false;
	}
	
	HttpClientAccessManager::RequestContext::RequestContext() :mCreateTimeUs(0), mTcpClient(NULL) {
	}
	HttpClientAccessManager::RequestContext::~RequestContext() {
		
	}
}
