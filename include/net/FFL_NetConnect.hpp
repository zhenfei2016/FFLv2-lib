/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetConnect.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  网络连接基类
*/
#ifndef _FFL_NETCONNECT_HPP_
#define _FFL_NETCONNECT_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>
#include <net/FFL_NetConnect.hpp>

namespace FFL {
	class NetConnectManager;
	class ConnectThread;
	class NetConnect {
		friend class NetConnectManager;
	public:
		NetConnect(NetFD fd);
		virtual ~NetConnect();
	public:
		NetFD getFd() const {
			return mFd;
		}
		//
		//  启动，停止connect
		//
		status_t start();
		void stop();
	public:
		//
		//  处理fd上数据的循环，返回是否继续处理
		//
		virtual bool process();
	protected:
		void setConnectManager(NetConnectManager* mgr) {
			mConnMgr = mgr;
		}
		NetConnectManager* getConnectManager() {
			return mConnMgr;
		}
		NetFD mFd;
		NetConnectManager* mConnMgr;
	protected:
		friend class ConnectThread;
		FFL::sp<ConnectThread> mProcessThread;
	};
	
}

#endif
