/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpConnManager.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http连接管理器，创建http连接，管理（添加，删除）所有的http连接
*
*/
#include <net/http/FFL_HttpConnManager.hpp>
#include <net/http/FFL_Http.hpp>
#include <utils/FFL_StringHelper.hpp>

namespace FFL {

	class DefHttpConnectHandler : public HttpConnectHandler {
	public:
		DefHttpConnectHandler(HttpConnectMgr* mgr):mMgr(mgr){

		}

		//
		//   网络请求处理回调
		//   conn:网络连接
		//   request:网络请求
		//
		virtual void onReceiveRequest(HttpConnect* conn, HttpRequest* request) {
			mMgr->receiveRequest(conn,request);
		}

		HttpConnectMgr* mMgr;
	};
	HttpConnectMgr::HttpConnectMgr():mHandler(NULL){
		mRequestId = 0;
		mDefHandler = new DefHttpConnectHandler(this);
	}
	HttpConnectMgr::~HttpConnectMgr() {
	}
	void HttpConnectMgr::setHandler(sp<HttpConnectHandler> handler) {
		mHandler = handler;
	}
	void HttpConnectMgr::setFileHandler(sp<HttpFileHandler> hander) {
		mFileHandler = hander;
	}
	NetConnect* HttpConnectMgr::onCreateConnect(NetFD fd, NetServer* srv) {
		HttpConnect* conn= new  HttpConnect(fd, mDefHandler);		
		return conn;
	}

	void HttpConnectMgr::onDestroyConnect(NetConnect* conn) {		
		delete conn;
	}
    //
    //  注册处理指定http ，请求的处理句柄
    //
    void HttpConnectMgr::registerApi(const String& pathAndQuery, sp<HttpApiHandler> handler){
		//std::map<String, wp<HttpApiHandler> >::iterator it = mApis.find(pathAndQuery);
		//if (it == mApis.end()) {

		//}	

		String key = stringLower(pathAndQuery);
		CMutex::Autolock l(mApisLock);
		wp<HttpApiHandler> wHandler = handler;
		mApis[key] = wHandler;
    }
    FFL::sp<HttpApiHandler> HttpConnectMgr::unregisterApi(const String& pathAndQuery){
		String key = stringLower(pathAndQuery);

		FFL::sp<HttpApiHandler> hander;
		CMutex::Autolock l(mApisLock);
		std::map<String, wp<HttpApiHandler> >::iterator it = mApis.find(key);
		if (it != mApis.end()) {
			hander= it->second.promote();
		}
         return hander;
    }
    FFL::sp<HttpApiHandler> HttpConnectMgr::getRegisterApi(const String& pathAndQuery){
		String key = stringLower(pathAndQuery);

		CMutex::Autolock l(mApisLock);
		std::map<String, wp<HttpApiHandler> >::iterator it = mApis.find(key);
		if (it != mApis.end()) {
			return it->second.promote();
		}
         return NULL;
    }
	//
	//   网络请求处理回调
	//   conn:网络连接
	//   request:网络请求
	//
	void HttpConnectMgr::receiveRequest(HttpConnect* conn, HttpRequest* request) {
		String path;
		request->getPath(path);
		
		String query;
		request->getQuery(query);

		//FFL::List<FFL::String> params;
		//request->getQueryParams(params);

        int64_t requestId=mRequestId++;
        FFL_LOG_DEBUG_TAG("http","request(%" lld64  "): %s?%s",
                          requestId,
                          path.c_str(),query.c_str());
        
		if (query.size() == 0) {
			//
			//  请求文件
			//
			onRequestFile(conn, path,request);
		}
		else {
			FFL::sp<HttpApiHandler> handler = getRegisterApi(path + "?" + query);
			if (!handler.isEmpty()) {				
				handler->onHttpQuery(conn, query, request);
			}
			else {
				if (mHandler.isEmpty()) {
					FFL_LOG_DEBUG_TAG("http", "not find handler  %s?%s  ",
						path.c_str(), query.c_str());
					sp<HttpResponse> res = conn->createResponse();
					res->finish();
				}
				else {
					mHandler->onReceiveRequest(conn, request);
					FFL_socketClose(conn->getFd());
					//conn->close();
				}
			}
		}
        FFL_LOG_DEBUG_TAG("http","complete(%" lld64 "): %s?%s  \r\n\r\n",
                          requestId,
                          path.c_str(),query.c_str());
	}

	void HttpConnectMgr::onRequestFile(HttpConnect* conn,const String& path, HttpRequest* request) {
		if (!mFileHandler.isEmpty()) {
			if (mFileHandler->onHttpFile(conn, path)) {
				return;
			}
		}

		//
		//  默认的处理过程
		//
		sp<HttpResponse> res = conn->createResponse();
		res->finish();
	}
}
