/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_UdpServer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  udp服务器
*
*/

#include <net/FFL_UdpServer.hpp>
#include <net/FFL_NetEventLoop.hpp>

namespace FFL {
	const static int64_t kEventLoopTimeout = 15000 * 1000;
	//
	//  udp缓冲区大小
	//
	const static int32_t KUdpBufferSize = 1024*10;

	UdpServer::UdpServer(const char* ip, uint16_t port,
		UdpServer::Callback* handler,
		const char* name) {
		mServerName= name ? name : "";
		mIP = ip ? ip :"";
		mPort = port;
		mEventHandler = new EventHandler(this);
		mEventLoop = new NetEventLoop(kEventLoopTimeout);				
	}
	UdpServer::~UdpServer() {
		FFL_SafeFree(mEventLoop);
		FFL_SafeFree(mEventLoop);
	}
	//
	//  调用。start，stop会触发onStart,onStop的执行
	//  onStart :表示准备开始了 ,可以做一些初始化工作
	//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
	//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
	//
	bool UdpServer::onStart() {
		if (!mServerSocket.createUdpServer(mIP.c_str(),mPort)) {
			return false;
		}
		mEventLoop->addFd(mServerSocket.getFd(), mEventHandler, NULL,NULL );

        //
        //启动事件循环
        //
		bool ret= mEventLoop->start(NULL);
		if (!ret) {
			
		}
		return ret;
	}
	void UdpServer::onStop() {		
		mEventLoop->stop();
	}
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool UdpServer::eventLoop(int32_t* waitTime) {
		bool bContinue=mEventLoop->eventLoop(waitTime);
		processTimeout();
		return bContinue;
	}
	//
	//  处理超时
	//
	void UdpServer::processTimeout() {
		int64_t now = FFL_getNowUs();		
		FFL::sp<ClientContext> context = NULL;
		if (context.isEmpty() ||
			context->mTimeoutUs < 0) {
			return;
		}

		if (now > context->mLastSendRecvTimeUs) {
			if (now - context->mLastSendRecvTimeUs > context->mTimeoutUs) {
				//onClientDestroyed(context.get());
			}
		}    
	}
	UdpServer::ClientContext::ClientContext(NetFD fd) : mFd(fd), mClient(fd) {
		mLastSendRecvTimeUs = FFL_getNowUs();
		//
		// 30秒没有数据就关闭
		//
		mTimeoutUs = 30 * 1000 * 1000;
	}
	
	void UdpServer::addClient(FFL::sp<ClientContext> contex) {

	}
	void UdpServer::removeClient(NetFD fd) {

	}
	
	bool UdpServer::onClientCreated(ClientContext* context) {

		return true;
	}
	void UdpServer::onClientDestroyed(ClientContext* client) {

	}
	bool UdpServer::onClientReceived(NetFD fd) {
		uint8_t buf[KUdpBufferSize] = {};
		size_t size = 0;
		if (FFL_OK == mServerSocket.read(buf, KUdpBufferSize, &size)) {
			char srcIP[32] = {};
			uint16_t srcPort=0;
			mServerSocket.getReadFromAddr(srcIP, &srcPort);


		}


		if (mHandler != NULL) {
			//return mHandler->onClientReceived(nullptr);
		}
		return true;
	}
	//
	//  返回是否还继续读写
	//  priv:透传数据
	//
	bool UdpServer::EventHandler::onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv) {
        if (!mServer->onClientReceived(fd)) {			
			return true;
		}		
		return true;
	}
}
