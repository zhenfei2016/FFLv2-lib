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
	NetConnect::NetConnect(NetFD fd) :mFd(fd) {
		mConnMgr = NULL;		
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
		return onStart();		
	}
	void NetConnect::stop() {
		onStop();
	}
	//
	//  关闭自己，调用后就不能使用这个实例了
	//
	status_t NetConnect::realese() {
		NetConnectManager* mgr = getConnectManager();
		if (mgr != NULL) {
			mgr->destroyConnect(getFd());
			return FFL_OK;
		}

		return FFL_FAILED;
	}

	NetFD NetConnect::getFd() const {
		return mFd;
	}

	class ConnectThread : public Thread {
	public:
		ConnectThread(TcpConnect* conn) :mConn(conn) {
		}
		virtual bool threadLoop() {
			bool ret = mConn->process();
			return ret;
		}

		TcpConnect* mConn;
	};

	TcpConnect::TcpConnect(NetFD fd):NetConnect(fd) {
		mProcessThread=new	ConnectThread(this);
	}
	TcpConnect::~TcpConnect() {

	}
	status_t TcpConnect::onStart() {
		mProcessThread->run("conn");
		return FFL_OK;
	}
	void TcpConnect::onStop() {
		mProcessThread->requestExitAndWait();
	}
}