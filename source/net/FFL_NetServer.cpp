/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetServer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  网路服务器基类
*
*/
#include <net/FFL_NetServer.hpp>
#include <net/FFL_NetConnect.hpp>
#include <net/FFL_NetConnectManager.hpp>
#include <net/base/FFL_Net.h>
#include <thread/FFL_Thread.hpp>
namespace FFL {
	NetServer::NetServer():mConnectMgr(NULL) {	
		FFL_socketInit();
	}
	NetServer::~NetServer() {		
	}
	//
	//  连接管理
	//
	void NetServer::setConnectManager(NetConnectManager* mgr) {
		mConnectMgr = mgr;
	}
	NetConnectManager* NetServer::getConnectManager() const {
		return mConnectMgr;
	}
	status_t NetServer::start() {	
		if (mConnectMgr == NULL) {
			FFL_LOG_ERROR("Failed to NetServer::start connectMgr =null");
			return FFL_FAILED;
		}
		return onStart();
	}
	//
	//  停止tcp服务器
	//
	void NetServer::stop() {
		onStop();
	}
	
	NetConnect* NetServer::createConnect(NetFD fd) {	
		NetConnectManager* mgr = getConnectManager();
		NetConnect* conn= mgr->createConnect(fd,this);
		return conn;
	}
	void NetServer::destroyConnect(NetFD fd) {
		return getConnectManager()->destroyConnect(fd);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ListenThread : public Thread {
	public:
		ListenThread(TcpServer* server) :mServer(server) {
		}
		virtual bool threadLoop() {
			return mServer->listenLoop();
		}

		TcpServer* mServer;
	};


	TcpServer::TcpServer(const char* ip,int32_t port) :mServerFd(-1){
		mIp = FFL_strdup(ip);
		mPort = port;
		mListenThread = new ListenThread(this);
	}
	TcpServer::~TcpServer() {
		mListenThread = NULL;
		FFL_free((void*)mIp);
		mIp = NULL;
	}
	//
	//  启动服务
	//		
	status_t TcpServer::onStart() {
		if (mServerFd >= 0) {
			return FFL_FAILED;
		}

		if (FFL_socketAnyAddrTcpServer(mPort, &mServerFd) != FFL_SOCKET_OK) {
			FFL_LOG_WARNING("Failed to create tcp server. %s:%d", mIp, mPort);
		}
		mListenThread->run("tcpserver");
		return FFL_OK;
	}
	//
	//  停止服务器
	//
	void TcpServer::onStop() {
		if (mServerFd >= 0) {
			mListenThread->requestExit();
			FFL_socketClose(mServerFd);
			mServerFd = -1;
			mListenThread->requestExitAndWait();
		}
	}
	//
	//  监听循环，返回是否继续监听
	//
	bool TcpServer::listenLoop() {
		if (mServerFd < 0) {
			return false;
		}
		
		NetFD clientFd=0;
		if (FFL_socketAccept(mServerFd, &clientFd) != FFL_SOCKET_OK){
			return false;
		}

		FFL_LOG_INFO("accept client fd=%d",clientFd);
		NetConnect* conn=createConnect(clientFd);
		if (conn) {
			conn->start();
		}
		return true;
	}
}
