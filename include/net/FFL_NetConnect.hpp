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
		//
		//  启动，停止connect
		//
		status_t start();
		void stop();
		//
		//  关闭自己，调用后就不能使用这个实例了
		//
		status_t realese();
	public:
		NetFD getFd() const;
	protected:
		virtual status_t onStart()=0;
		virtual void onStop()=0;
	protected:
		void setConnectManager(NetConnectManager* mgr) {
			mConnMgr = mgr;
		}
		NetConnectManager* getConnectManager() {
			return mConnMgr;
		}
		NetFD mFd;
		NetConnectManager* mConnMgr;	
	};

	class TcpConnect : public NetConnect {
	public:
		TcpConnect(NetFD fd);
		~TcpConnect();
	public:
		virtual status_t onStart() ;
		virtual void onStop() ;
		//
		//  处理fd上数据的循环，返回是否继续处理
		//
		virtual bool process()=0;
	protected:
		friend class ConnectThread;
		FFL::sp<ConnectThread> mProcessThread;
	};
	
}

#endif
