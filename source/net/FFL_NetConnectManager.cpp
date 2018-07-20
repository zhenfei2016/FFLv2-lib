/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetConnectManager.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  连接管理器，创建保存连接
*/
#include <net/FFL_NetConnect.hpp>
#include <net/FFL_NetConnectManager.hpp>

namespace FFL {
	NetConnectManager::NetConnectManager(){		
	}
	NetConnectManager::~NetConnectManager(){
	}
	void NetConnectManager::addConnect(NetConnect* conn) {
		if (conn == NULL) {
			return;
		}

		CMutex::Autolock l(mConnLock);
		for (List<NetConnect*>::iterator it = mConnects.begin(); it != mConnects.end(); it++) {
			if (*it == conn) {
				return;
			}
		}

		conn->setConnectManager(this);
		mConnects.push_back(conn);
	}
	NetConnect* NetConnectManager::removeConnect(NetFD fd) {
		CMutex::Autolock l(mConnLock);

		for (List<NetConnect*>::iterator it = mConnects.begin(); it != mConnects.end(); it++) {
			NetConnect* conn = *it;
			if (conn->getFd() == fd) {
				mConnects.erase(it);
				return conn;
			}
		}

		return NULL;
	}

}