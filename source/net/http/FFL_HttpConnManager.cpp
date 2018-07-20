/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpConnManager.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http连接管理器，创建http连接，管理（添加，删除）所有的http连接
*
*/
#include <net/http/FFL_HttpConnect.hpp>
#include <net/http/FFL_HttpConnManager.hpp>

namespace FFL {
	HttpConnectMgr::HttpConnectMgr():mHandler(NULL){
	}
	HttpConnectMgr::~HttpConnectMgr() {
	}
	void HttpConnectMgr::setHandler(sp<HttpConnectHandler> handler) {
		mHandler = handler;
	}
	NetConnect* HttpConnectMgr::createConnect(NetFD fd, NetServer* srv) {
		HttpConnect* conn= new  HttpConnect(fd, mHandler);
		addConnect(conn);
		return conn;

	}
	void HttpConnectMgr::destroyConnect(NetFD fd) {
		NetConnect* conn = removeConnect(fd);
		delete conn;
	}

}
