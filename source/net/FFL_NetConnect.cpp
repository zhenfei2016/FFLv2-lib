/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetConnect.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  网络连接基类
*/
#include <net/base/FFL_Net.h>
#include <net/FFL_NetConnect.hpp>
#include <net/FFL_NetConnectManager.hpp>
#include <thread/FFL_Thread.hpp>


namespace FFL {
	class ConnectThread : public Thread {
	public:
		ConnectThread(NetConnect* conn) :mConn(conn) {
		}
		virtual bool threadLoop() {
			bool ret= mConn->process();
			return ret;
		}

		NetConnect* mConn;
	};

	NetConnect::NetConnect(NetFD fd) :mFd(fd) {
		mConnMgr = NULL;
		mProcessThread = new ConnectThread(this);
	}
	NetConnect::~NetConnect() {		
		FFL_LOG_INFO("remove conn fd=%d",mFd);
		if (mFd >= 0) {
			FFL_socketClose(mFd);			
			mFd = -1;
		}
	}
	//
	//  启动，停止connect
	//
	status_t NetConnect::start() {
		mProcessThread->run("conn");
		return FFL_OK;
	}
	void NetConnect::stop() {
		mProcessThread->requestExitAndWait();
	}
	//
	//  处理fd上数据的循环，返回是否继续处理
	//
	bool NetConnect::process() {
		return false;
	}
}