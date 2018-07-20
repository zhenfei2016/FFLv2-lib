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
namespace FFL {
	class HttpConnectMgr : public NetConnectManager {
	public:
		HttpConnectMgr();
		~HttpConnectMgr();

		void setHandler(sp<HttpConnectHandler> handler);

		virtual NetConnect* createConnect(NetFD fd, NetServer* srv);
		virtual void destroyConnect(NetFD fd);
	private:

		sp<HttpConnectHandler> mHandler;
	};
}

#endif