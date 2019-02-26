/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_WebSocketServer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2019/02/24
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  websocket服务器
*  
*  协议解析
*  https://blog.csdn.net/vsrfind/article/details/79268411
*  https://tools.ietf.org/html/rfc6455
*/

#include <net/websocket/FFL_WebSocketServer.hpp>
#include <net/http/FFL_HttpUrl.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/FFL_TcpClient.hpp>
#include <net/FFL_TcpServer.hpp>
#include <net/http/FFL_HttpClient.hpp>
#include <net/websocket/FFL_WebSocketClient.hpp>
#include <list>
#include "FFL_WebSocketHandshark.hpp"

namespace FFL {
	class WebSocketAcceptClient : public RefBase {
	public:
		WebSocketAcceptClient(TcpClient* client):mClient(client){
		}

		FFL::sp<WSHandsharkRequest> readHandsharkRequest(){
			//FFL::sp<HttpResponse> request = client->readRequest();
			return NULL;
		}

		//
		//  接收一帧数据
		//  buffer: 缓冲区 ， 
		//  bufferSize：这个输入缓冲区的大小。 当成功接收数据的情况下返回接收的数据大小
		//
		bool recvFrame(uint8_t* buffer, uint32_t* bufferSize) {
			return false;
		}
		//
		//  发送一帧数据
		//
		bool sendFrame(const uint8_t* data, uint32_t len) {
			return false;
		}
	private:
		TcpClient* mClient;
	};

	//
	//  具体服务器实现
	//
	class WebSocketServerImpl :public TcpServer::Callback{
		friend class WebSocketServer;
		public:
			WebSocketServerImpl(const char* ip, int32_t port, WebSocketServer::Callback* callback);
			virtual ~WebSocketServerImpl();
	    protected:		
			//
			//  调用。start，stop会触发onStart,onStop的执行
			//  onStart :表示准备开始了 ,可以做一些初始化工作
			//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
			//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
			//
			FFL_INLINE bool onStart();
			FFL_INLINE void onStop();
			//
			//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
			//   waitTime:输出参数，下一次执行eventLoop等待的时长
			//   true  : 继续进行下一次的eventLoop
			//   false : 不需要继续执行eventloop
			//
			FFL_INLINE bool eventLoop(int32_t* waitTime);
		private:	
			// class TcpServer::Callback
			//
			//  aliveTimeUs:保活时长，如果超过这么长时间还没有数据则干掉这个client
			//              <0 一直存活， 
			//
			virtual bool onClientCreate(TcpClient* client, int64_t* aliveTimeUs);
			virtual void onClientDestroy(TcpClient* client, TcpServer::Callback::FD_OPTMODE reason);
			virtual TcpServer::Callback::FD_OPTMODE onClientReceived(TcpClient* client);
	    private:
			TcpServer* mTcpServer;	
			WebSocketServer::Callback* mCallback;
	};

	WebSocketServerImpl::WebSocketServerImpl(const char* ip, int32_t port, WebSocketServer::Callback* callback):
		mCallback(callback){
		mTcpServer = new TcpServer(ip,port, this);
	}
	WebSocketServerImpl::~WebSocketServerImpl(){
		FFL_SafeFree(mTcpServer);		
	}


	//
	//  内部保存的一条http连接上，的一下相关信息
	//
	class WebsocketContext {
	public:
		WebsocketContext(TcpClient* client);
		~WebsocketContext();

		TcpClient* mTcpClient;
		FFL::sp<WebSocketAcceptClient> mWebsocket;

		//
		//  是否已经握手成功了
		//
		bool mHandshake;
	};
	WebsocketContext::WebsocketContext(TcpClient* client):mTcpClient(client){
		mWebsocket = new WebSocketAcceptClient(client);
		mHandshake = false;
	}
	WebsocketContext::~WebsocketContext() {				
		mTcpClient = NULL;
	}
	//
	//  一条http连接开始建立
	//
	bool WebSocketServerImpl::onClientCreate(TcpClient* client, int64_t* aliveTimeUs) {
		WebsocketContext* contex = new WebsocketContext(client);
		client->setUserdata(contex);
		return contex->mHandshake;
	}
	void WebSocketServerImpl::onClientDestroy(TcpClient* client, TcpServer::Callback::FD_OPTMODE reason) {
		WebsocketContext* contex = (WebsocketContext*)client->getUserdata();
		client->setUserdata(NULL);
		FFL_SafeFree(contex);
	}
	//
	//  client上有数据可以读了
	//
	TcpServer::Callback::FD_OPTMODE WebSocketServerImpl::onClientReceived(TcpClient* client) {
		WebsocketContext* contex = (WebsocketContext*)client->getUserdata();
		if (!contex->mHandshake) {
			//
			//  握手请求
			//
			FFL::sp<WSHandsharkRequest> request = contex->mWebsocket->readHandsharkRequest();
			if (request.isEmpty()) {
				return TcpServer::Callback::FD_DESTROY;
			}

			String key;
			if (!request->getSecKey(key)) {
				return TcpServer::Callback::FD_DESTROY;
			}

			FFL::sp<WSHandsharkResponse> response =
				new WSHandsharkResponse(request->getHttpClient(), key);
			response->send();			
			contex->mHandshake = true;
			return TcpServer::Callback::FD_CONTINUE;
		}
		//
		//  读数据
		//
		if (!contex->mWebsocket->recvFrame(NULL,NULL)) {
			return TcpServer::Callback::FD_DESTROY;
		}
		
		return TcpServer::Callback::FD_CONTINUE;		
	}
	
	//
	//  调用。start，stop会触发onStart,onStop的执行
	//  onStart :表示准备开始了 ,可以做一些初始化工作
	//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
	//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
	//
	FFL_INLINE bool WebSocketServerImpl::onStart() {
		mTcpServer->start(NULL);
		return true;
	}
	FFL_INLINE void WebSocketServerImpl::onStop() {
		mTcpServer->stop();
	}	
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	FFL_INLINE bool WebSocketServerImpl::eventLoop(int32_t* waitTime) {
		return mTcpServer->eventLoop(waitTime);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	WebSocketServer::WebSocketServer(const char* ip, int32_t port, Callback* callback) {
		mImpl = new WebSocketServerImpl(ip, port,callback);
	}
	WebSocketServer::~WebSocketServer() {
		FFL_SafeFree(mImpl);
	}	
	//
	//  调用。start，stop会触发onStart,onStop的执行
	//  onStart :表示准备开始了 ,可以做一些初始化工作
	//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
	//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
	//
	bool WebSocketServer::onStart() {
		return mImpl->onStart();
	}
	void WebSocketServer::onStop() {
		mImpl->onStop();
	}	
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool WebSocketServer::eventLoop(int32_t* waitTime) {
		return mImpl->eventLoop(waitTime);
	}
}
