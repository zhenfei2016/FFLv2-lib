/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_WebSocketClient.hpp
*  Created by zhufeifei(34008081@qq.com) on 2019/02/24
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  用于websocket客户端连接 websocket服务器，然后进行发送接收数据桢
*/

#ifndef _FFL_WEBSOCKET_CLIENT_HPP_
#define _FFL_WEBSOCKET_CLIENT_HPP_

#include <FFL_RefBase.hpp>
#include <net/FFL_TcpClient.hpp>
#include <net/FFL_NetStream.hpp>

namespace FFL {		
	
	class FFLIB_API_IMPORT_EXPORT WebSocketClient : public RefBase{
	public:
		WebSocketClient();
		virtual ~WebSocketClient();
	public:
		//
		//  连接host
		//
		bool connect(const char* host);
		bool connect(const char* ip,uint16_t port);
		bool isConnected() const;
		//
		//  关闭连接
		//
		void close();
		//
		//  发送握手请求，并等待握手成功应答
		//
		bool handshark(const char* path);	
		bool isHandshark() const;
		//
		//  接收一帧数据
		//  buffer: 缓冲区 ， 
		//  bufferSize：这个输入缓冲区的大小。 当成功接收数据的情况下返回接收的数据大小
		//
		bool recvFrame(uint8_t* buffer,uint32_t* bufferSize);
		//
		//  发送一帧数据
		//
		bool sendFrame(const uint8_t* data,uint32_t len);
	private:
		TcpClient* mClient;
		NetStreamReader* mStream;
		String mHost;

		//
		//  是否已经连接成功了
		//
		volatile bool mIsConnected;
		// 
		//  是否已经握手成功了
		//
		volatile bool mIsHandShark;
	};
}
#endif
