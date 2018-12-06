/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetTcpServer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  网路服务器基类
*
*/
#include <net/FFL_NetTcpServer.hpp>
#include <net/base/FFL_Net.h>

namespace FFL {
	NetTcpServer::NetTcpServer(const char* ip, uint16_t port, TcpServerListener* listener) :mServerFd(0),mTcpListener(listener) {		
		memcpy(mIP, ip, FFL_MIN((strlen(ip)+1), 32));
		mPort = port;
	}
	NetTcpServer::~NetTcpServer() {				
	}
	//
	//  调用。start，stop会触发onStart,onStop的执行
	//  onStart :表示准备开始了 ,可以做一些初始化工作
	//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
	//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
	//
	bool NetTcpServer::onStart() {
		if (mServerFd > 0) {
			return false;
		}

		mServerFd = 0;
		if (FFL_socketAnyAddrTcpServer(mPort, &mServerFd) != FFL_SOCKET_OK) {
			FFL_LOG_WARNING("Failed to create tcp server. %s:%d", mIP, mPort);
		}
		return true;	
	}
	void NetTcpServer::onStop() {
		if (mServerFd > 0) {			
			FFL_socketClose(mServerFd);
			mServerFd = -1;			
		}
	}

	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool NetTcpServer::eventLoop(int32_t* waitTime){
		if (mServerFd < 0) {
			FFL_LOG_ERROR("Faile to NetTcpServer::eventLoop serverFd=NULL");
			return false;
		}

		NetFD clientFd = 0;
		if (FFL_socketAccept(mServerFd, &clientFd) != FFL_SOCKET_OK) {
			FFL_LOG_ERROR("Faile to NetTcpServer::FFL_socketAccept");
			return false;
		}

		if (mTcpListener) {
			mTcpListener->onAcceptClient(clientFd);
		}
		FFL_LOG_INFO("accept client fd=%d", clientFd);		

		if (waitTime) {
			*waitTime = 0;
		}
		return true;
	}
}