/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_UdpServer.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  udp服务器
*
*/
#ifndef _FFL_UDP_SERVER_HPP_
#define _FFL_UDP_SERVER_HPP_

#include <net/FFL_NetEventLoop.hpp>
#include <net/FFL_UdpClient.hpp>

namespace FFL {    
	class UdpServer : public Module{
	public:
		class Callback {
		public:
			//
			//  aliveTimeUs:保活时长，如果超过这么长时间还没有数据则干掉这个client
			//              <0 一直存活， 
			//
			virtual bool onClientCreate(UdpClient* client,int64_t* aliveTimeUs) = 0;
			virtual void onClientDestroy(UdpClient* client, int reason) = 0;
			virtual bool onClientReceived(UdpClient* client,const char* packet,size_t size) = 0;

		};
	public:
        //
        //   ip:服务地址
        //   port: 服务端口
        //   handler ：处理句柄
        //   name:  服务名称
        //
		UdpServer(const char* ip,uint16_t port,
			UdpServer::Callback* handler,
			      const char* name=NULL);
		virtual ~UdpServer();

	protected:

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
		virtual bool eventLoop(int32_t* waitTime) ;
	private:
		//
		//  处理超时
		//
		void processTimeout();
	protected:
		std::string mServerName;
		std::string mIP;
		uint16_t mPort;
		FFL::CSocket mServerSocket;

		class ClientContext :public FFL::RefBase {
		public:
			ClientContext(NetFD fd);
			NetFD mFd;
			UdpClient mClient;
			//
			// 最后一次发送接受的时间点
			//
			int64_t mLastSendRecvTimeUs;
			//
			// 多长时间没有数据就关闭这个连接
			//
			int64_t mTimeoutUs;
		};

		CMutex mCLientsLock;
		std::list< FFL::sp<ClientContext> > mClients;
		void addClient(FFL::sp<ClientContext> contex);
		void removeClient(NetFD fd);

		virtual bool onClientCreated(ClientContext* context);
		virtual void onClientDestroyed(ClientContext* client);
		virtual bool onClientReceived(NetFD fd);
		Callback* mHandler;	
	private:
		//
		//  监听socket是否有可读的消息了
		//
		class EventHandler : public NetEventLoop::Callback {
		public:
			EventHandler(UdpServer* server) :mServer(server){}
			//
			//  返回是否还继续读写
			//  priv:透传数据
			//
			virtual bool onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv);
			UdpServer* mServer;
		};
		NetEventLoop* mEventLoop;
		EventHandler* mEventHandler;		
	
        
	};
}
#endif
