/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetTcpServer.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*  socket读写
*
*/
#ifndef _FFL_NEY_TCP_SERVER_HPP_
#define _FFL_NEY_TCP_SERVER_HPP_

#include <utils/FFL_Module.hpp>
namespace FFL {
	class TcpServerListener {
	public:
		virtual void onAcceptClient(NetFD fd) = 0;
	};

	class NetTcpServer : public Module {
	public:
		NetTcpServer(const char* ip, uint16_t port, TcpServerListener* listener);
		~NetTcpServer();
	public:
		//
		//  调用。start，stop会触发onStart,onStop的执行
		//  onStart :表示准备开始了 ,可以做一些初始化工作
		//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
		//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
		//
		virtual bool onStart();
		virtual void onStop();

		//
		//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
		//   waitTime:输出参数，下一次执行eventLoop等待的时长
		//   true  : 继续进行下一次的eventLoop
		//   false : 不需要继续执行eventloop
		//
		virtual bool eventLoop(int32_t* waitTime);	
	private:
		char     mIP[32];
		uint16_t mPort;
		NetFD    mServerFd;

		TcpServerListener* mTcpListener;
	};
}
#endif
