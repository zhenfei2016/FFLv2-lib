/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpConnManager.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http连接管理器，创建http连接，管理（添加，删除）所有的http连接
*
*/
#ifndef _FFL_NETCONNECT_MANAGER_HTTP_HPP_
#define _FFL_NETCONNECT_MANAGER_HTTP_HPP_

#include <net/FFL_NetConnectManager.hpp>
#include <net/http/FFL_HttpConnHandle.hpp>
#include <utils/FFL_Map.hpp>
#include <thread/FFL_Mutex.hpp>

namespace FFL {
	class HttpConnectMgr : public NetConnectManager {
	public:
		HttpConnectMgr();
		~HttpConnectMgr();

		//
		//  创建，删除http连接
		//
		virtual NetConnect* onCreateConnect(NetFD fd, NetServer* srv);
		virtual void onDestroyConnect(NetConnect* conn);

		//
		//  设置http处理句柄
		//
		void setHandler(sp<HttpConnectHandler> handler);
	public:
		//
		//  注册处理指定http ，请求的处理句柄
		//
		void registerApi(const String& pathAndQuery, sp<HttpApiHandler> handler);
		sp<HttpApiHandler> unregisterApi(const String& pathAndQuery);
		sp<HttpApiHandler> getRegisterApi(const String& pathAndQuery);

	protected:
		//
		//   网络请求处理回调
		//   conn:网络连接
		//   request:网络请求
		//
		friend class DefHttpConnectHandler;
		void receiveRequest(HttpConnect* conn, HttpRequest* request);
	private:
		//
		//  默认的handler;
		//
		sp<HttpConnectHandler> mDefHandler;

		sp<HttpConnectHandler> mHandler;

		CMutex mApisLock;
		std::map<String, wp<HttpApiHandler> > mApis;
        
        int64_t mRequestId;
	};
}

#endif
