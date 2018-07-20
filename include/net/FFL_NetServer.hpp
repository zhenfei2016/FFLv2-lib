/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetServer.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14 
*  https://github.com/zhenfei2016/FFL-v2.git
*  网路服务器基类
*
*/
#ifndef _FFL_NET_SERVER_HPP_
#define _FFL_NET_SERVER_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>

namespace FFL {
	class NetConnect;
	class NetConnectManager;
	class NetServer {
	public:
		NetServer();
		virtual ~NetServer();
		//
		//  连接管理
		//
		void setConnectManager(NetConnectManager* mgr);
		NetConnectManager* getConnectManager() const;
		//
		//  启动,停止服务,启动服务前需要setConnectManager
		//		
		status_t start();
		void stop();
	protected:		
		//
		//  启动,停止服务实现
		//		
		virtual status_t onStart()=0;
		virtual void onStop()=0;
	protected:
	    //
		//  创建，删除一个网络连接
		//
		NetConnect* createConnect(NetFD fd);
		void destroyConnect(NetFD fd);
	private:
		NetConnectManager* mConnectMgr;	
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class ListenThread;
	class TcpServer : public NetServer {
	public:
		TcpServer(const char* ip,int32_t port);		
		~TcpServer();
		//
		//  启动,停止服务实现
		//		
		virtual status_t onStart();
		virtual void onStop();
	protected:
		//
		//  监听循环，返回是否继续监听
		//
		bool listenLoop() ;
	private:	
		const char* mIp;
		int32_t mPort;
		int32_t mServerFd;

		friend class ListenThread;
		FFL::sp<ListenThread> mListenThread;

	};
}

#endif
