/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_WebSocketClient.cpp
*  Created by zhufeifei(34008081@qq.com) on 2019/02/24
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  websocket客户端
*/

#include <net/websocket/FFL_WebSocketClient.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/FFL_NetEventLoop.hpp>
#include "FFL_WebSocketHandshark.hpp"

namespace FFL {
	WebSocketClient::WebSocketClient() :
		mClient(NULL),
		mStream(NULL),
		mIsConnected(false),
		mIsHandShark(false) {
	}
	WebSocketClient::~WebSocketClient() {	
		close();		
	}
	//
	//  连接host
	//
	bool WebSocketClient::connect(const char* host) {	
		mHost = host;
		return connect("127.0.0.1",80);
	}
	bool WebSocketClient::connect(const char* ip, uint16_t port) {
		if (isConnected()) {
			return false;
		}

		TcpClient* client = new TcpClient();
		if (TcpClient::connect(ip,port, *client) != FFL_OK) {
			FFL_SafeFree(client);
			return false;
		}

		mClient = client;
		mIsConnected = true;
		if (mHost.isEmpty()) {
			mHost = String::format("%s:%d", ip, port);
		}
		return true;
	}
	bool WebSocketClient::isConnected() const {
		return mIsConnected;
	}
	//
	//  关闭连接
	//
	void WebSocketClient::close() {	
		if (mIsConnected) {
			mClient->close();
			mIsConnected = false;
			FFL_SafeFree(mClient);
		}
		mIsHandShark = false;
		FFL_SafeFree(mStream);
	}
	//
	//  发送握手请求，并等待握手成功应答
	//
	bool WebSocketClient::handshark(const char* path) {
		if (isHandshark()) {
			return false;
		}

		//
		// 发送握手请求
		//
		sp<HttpClient> httpClient = new HttpClient(mClient);
		FFL::HttpHeader header;		
		header.setKey("Host", mHost.string());		
		//header.setKey("Origin", host.string());
		FFL::sp<WSHandsharkRequest> request = new FFL::WSHandsharkRequest(httpClient);
		request->setHeader(header);
		if (!request->send()) {
			return false;
		}

		//
		//  等待握手应答
		//		
		FFL::sp<HttpResponse> response=httpClient->readResponse();
		if (response.isEmpty()) {
			return false;
		}		
		if (!WSHandsharkResponse::isWebSocket(response.get())) {
			return false;
		}
		//
		//  验证key是否正确
		//
		if(!WSHandsharkResponse::isHandsharkOk(request.get(),response.get())){
			return false;
		}

		mStream = new NetStreamReader(mClient);
		mIsHandShark = true;		
		return true;
	}
	bool WebSocketClient::isHandshark() const {
		return mIsHandShark;
	}

	class WebsocketFrame {
	public:
		WebsocketFrame() {
			reset();
		}
		~WebsocketFrame() {
		}

		void reset() {
			FIN = false;
			mOpcode = 0;
			mHaveMask = false;
			mPayloadLen = 0;
			mMaskey[0] = 0;
			mMaskey[1] = 0;
			mMaskey[2] = 0;
			mMaskey[3] = 0;
		}

		bool readHeader(NetStreamReader* reader) {
			bool suc = false;
			uint8_t b=reader->read1Bytes(&suc);
			if (!suc) {
				return false;
			}
			FIN =(b & 0x80)!=0;
			mOpcode = b & 0x0f;

			b = reader->read1Bytes(&suc);
			if (!suc) {
				return false;
			}
			mHaveMask = (b & 0x80)!=0;
			uint8_t payloadLenFlag= b & 0x7f;
			if (payloadLenFlag <= 125) {
				mPayloadLen = payloadLenFlag;
			}else if (payloadLenFlag == 126) {
				mPayloadLen = reader->read2Bytes(&suc);
			}else if (payloadLenFlag == 127) {
				mPayloadLen = reader->read8Bytes(&suc);
			}

			if (mHaveMask) {
				mMaskey[0] = reader->read1Bytes(&suc);
				mMaskey[1] = reader->read1Bytes(&suc);
				mMaskey[2] = reader->read1Bytes(&suc);
				mMaskey[3] = reader->read1Bytes(&suc);
			}

			return true;
		}
		bool readData(NetStreamReader* reader, uint8_t* buffer, uint32_t* bufferSize) {
			if (mPayloadLen > *bufferSize) {
				return false;
			}

			if (!reader->readBytes((int8_t*)buffer, mPayloadLen)) {
				*bufferSize = mPayloadLen;
				return false;
			}

			return true;
		}

		//
		//  服务端到客户端必须不添加掩码
		//  客户端到服务端必须添加掩码
		//

		//   
		//  第1byte
		//
		//  这个消息的最后一个数据包  1bit
		//
		bool FIN;
		//
		//    保留3bit
		//
		// 消息类型，4bit
		// 0x0：中间数据包，0x1：text类型，
		// 0x2: 二进制类型  0x8：断开连接  
		// 0x9: ping        0xa:pong 
		//
		uint8_t mOpcode;

		//
		//  第2byte
		//
		//
		//  是否添加掩码了 ,1bit
		//
		bool mHaveMask;
		//
		//  0-125 1字节，7bit
		//  126  2字节
		//  127  8字节 
		int64_t  mPayloadLen;
		//
		//  mask key用于跟数据xor
		//
		uint8_t mMaskey[4];
	};

	//
	//  接收一帧数据
	//  buffer: 缓冲区 ， 
	//  bufferSize：这个输入缓冲区的大小。 当成功接收数据的情况下返回接收的数据大小
	//
	bool WebSocketClient::recvFrame(uint8_t* buffer, uint32_t* bufferSize) {
		if (!isHandshark()) {
			return false;
		}

		WebsocketFrame frame;		
		uint8_t buf[4096] = {};
		size_t readed = 0;
		if (!frame.readHeader(mStream)) {
			return false;
		}

		//
		//  服务端下发的数据不进行xor
		//
		if (frame.mHaveMask) {
			return false;
		}

		if (!frame.readData(mStream, buffer, bufferSize)) {
			return false;
		}
		return true;
	}
	//
	//  发送一帧数据
	//
	bool WebSocketClient::sendFrame(const uint8_t* data, uint32_t len) {
		if (!isHandshark()) {
			return false;
		}

		return false;
	}

	
}
