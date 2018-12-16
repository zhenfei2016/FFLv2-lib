/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpServer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/11/08
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http服务器，在上一版本基础上修改的
*/

#include <net/http/FFL_HttpServer.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpUrl.hpp>
#include <net/FFL_TcpClient.hpp>
#include <net/FFL_TcpServer.hpp>
#include <net/http/FFL_HttpClient.hpp>

namespace FFL {

	HttpServer::HttpServer(const char* ip, int32_t port){
		mTcpCallback = new TcpServerCallback(this);
		mTcpServer = new TcpServer(ip,port, mTcpCallback);
	}
	HttpServer::~HttpServer(){
		FFL_SafeFree(mTcpServer);
		FFL_SafeFree(mTcpCallback);
	}
	//
	//  注册处理指定http ，请求的处理句柄
	//
	void HttpServer::registerApi(const HttpApiKey& key, FFL::sp<HttpServer::Callback> handler) {
		if (handler == NULL) {
			return;
		}

		ApiEntry* entry = new ApiEntry();
		entry->mKey = key;
		entry->mHandler = handler;

		FFL::CMutex::Autolock l(mApiListLock);
		mApiList.push_back(entry);
	}
	FFL::sp<HttpServer::Callback> HttpServer::unregisterApi(const HttpApiKey& key) {
		FFL::CMutex::Autolock l(mApiListLock);
		std::list<ApiEntry*>::iterator it = mApiList.begin();
		for (; it != mApiList.end(); it++) {
			ApiEntry* entry = *it;
			if (entry->mKey.mPath== key.mPath && entry->mKey.mQuery == key.mQuery ) {
				mApiList.erase(it);
				return entry->mHandler;
			}
		}
		return NULL;
	}
	FFL::sp<HttpServer::Callback> HttpServer::getRegisterApi(const HttpApiKey& key) {
		FFL::CMutex::Autolock l(mApiListLock);
		std::list<ApiEntry*>::iterator it = mApiList.begin();
		for (; it != mApiList.end(); it++) {
			ApiEntry* entry = *it;
			if (entry->mKey.mPath == key.mPath && entry->mKey.mQuery == key.mQuery) {
				return entry->mHandler;
			}
		}
		return NULL;
	}
	HttpServer::HttpContext::HttpContext(TcpClient* client):mTcpClient(client){
		mHttpClient = new HttpClient(client);
	}
	HttpServer::HttpContext::~HttpContext() {				
		mTcpClient = NULL;

	}
	bool HttpServer::onHttpClientCreate(TcpClient* client, int64_t* aliveTimeUs) {
		HttpContext* contex = new HttpContext(client);
		client->setUserdata(contex);
		return true;
	}
	void HttpServer::onHttpClientDestroy(TcpClient* client, int reason) {
		HttpContext* contex = (HttpContext*)client->getUserdata();
		client->setUserdata(NULL);
		FFL_SafeFree(contex);
	}
	bool HttpServer::onHttpClientReceived(TcpClient* client) {
		HttpContext* contex = (HttpContext*)client->getUserdata();
		FFL::sp<HttpRequest> request=contex->mHttpClient->readRequest();
		if (!request.isEmpty()) {
			if (!processHttpRequest(request)) {
				//
				//  关闭这个连接
				//

				return false;
			}

			return true;
		}	
		return false;
	}

	//
	//  处理http请求
	//
	bool HttpServer::processHttpRequest(FFL::sp<HttpRequest> request) {
		HttpUrl url;
		request->getUrl(url);

		HttpApiKey key;
		key.mPath = url.mPath;
		key.mQuery = url.mQuery;
		FFL::sp<HttpServer::Callback> apiHandler=getRegisterApi(key);
		if (!apiHandler.isEmpty()) {
			return apiHandler->onHttpQuery(request.get());
		} else {
			//
			//  404错误
			//
			FFL::sp<HttpResponse> response=request->makeResponse();
			response->setStatusCode(404);
			response->send();
			return false;
		}
		return true;
	}
	//
	//  调用。start，stop会触发onStart,onStop的执行
	//  onStart :表示准备开始了 ,可以做一些初始化工作
	//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
	//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
	//
	bool HttpServer::onStart() {
		mTcpServer->start(NULL);
		return true;
	}
	void HttpServer::onStop() {
		mTcpServer->stop();
	}
	
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool HttpServer::eventLoop(int32_t* waitTime) {
		return mTcpServer->eventLoop(waitTime);
	}

	HttpServer::TcpServerCallback::TcpServerCallback(HttpServer* server):
	          mServer(server){
	}
	//
	//  aliveTimeUs:保活时长，如果超过这么长时间还没有数据则干掉这个client
	//              <0 一直存活， 
	//
	bool HttpServer::TcpServerCallback::onClientCreate(TcpClient* client, int64_t* aliveTimeUs) {
		return mServer->onHttpClientCreate(client, aliveTimeUs);
	}
	void HttpServer::TcpServerCallback::onClientDestroy(TcpClient* client, int reason) {
		mServer->onHttpClientDestroy(client,reason);
	}
	bool HttpServer::TcpServerCallback::onClientReceived(TcpClient* client) {
		return mServer->onHttpClientReceived(client);
	}
}
