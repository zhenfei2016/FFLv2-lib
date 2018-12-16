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
*  http������������һ�汾�������޸ĵ�
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
	//  ע�ᴦ��ָ��http ������Ĵ�����
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
				//  �ر��������
				//

				return false;
			}

			return true;
		}	
		return false;
	}

	//
	//  ����http����
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
			//  404����
			//
			FFL::sp<HttpResponse> response=request->makeResponse();
			response->setStatusCode(404);
			response->send();
			return false;
		}
		return true;
	}
	//
	//  ���á�start��stop�ᴥ��onStart,onStop��ִ��
	//  onStart :��ʾ׼����ʼ�� ,������һЩ��ʼ������
	//  onStop :��ʾ׼��ֹͣ�� ,������ֹͣǰ��׼��������һ���ź���eventloop��ס�� 
	//  ����Щ�����У���Ҫ�ٵ����Լ��ĺ��������磺start,stop, isStarted��
	//
	bool HttpServer::onStart() {
		mTcpServer->start(NULL);
		return true;
	}
	void HttpServer::onStop() {
		mTcpServer->stop();
	}
	
	//
	//   �������߳���ִ�е�eventloop,�����Ƿ��������eventLoop
	//   waitTime:�����������һ��ִ��eventLoop�ȴ���ʱ��
	//   true  : ����������һ�ε�eventLoop
	//   false : ����Ҫ����ִ��eventloop
	//
	bool HttpServer::eventLoop(int32_t* waitTime) {
		return mTcpServer->eventLoop(waitTime);
	}

	HttpServer::TcpServerCallback::TcpServerCallback(HttpServer* server):
	          mServer(server){
	}
	//
	//  aliveTimeUs:����ʱ�������������ô��ʱ�仹û��������ɵ����client
	//              <0 һֱ�� 
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
