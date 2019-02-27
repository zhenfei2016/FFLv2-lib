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
#include "FFL_WebSocketFrame.hpp"

namespace FFL {
	static void xorBytes(const uint8_t *in1, uint8_t *out, uint8_t maker[4], int size) {
		for (int i = 0; i < size; i++) {
			*out = in1[i] ^ maker[i % 4];
			out++;
		}
	}

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
	NetFD WebSocketClient::getFd() {
		return mClient->getFd();
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
	bool WebSocketClient::sendFrame(uint8_t opcode,const uint8_t* data, uint32_t len) {
		if (!isHandshark()) {
			return false;
		}

		WebsocketFrame frame;
		frame.FIN = true;
		frame.mOpcode = opcode;
		frame.mHaveMask = true;
		frame.mPayloadLen = len;
		frame.writeHeader(mClient);
		if (len > 0) {
			//
		    //  xor
			//
			mClient->write((void*)data, len, 0);
		}
		return true;
	}
	bool WebSocketClient::sendText(const char* text) {		
		return sendFrame(WebsocketFrame::OPCODE_TEXT, (uint8_t*)text,strlen(text));
	}
	bool WebSocketClient::sendBinary(const uint8_t* data, uint32_t len) {
		return sendFrame(WebsocketFrame::OPCODE_BINARY, data, len);
	}
	bool WebSocketClient::sendStream(IOReader* stream) {
		FFL_ASSERT_LOG(0, "sendStream not implement");
		return false;
	}

	bool WebSocketClient::sendPing() {
		return sendFrame(WebsocketFrame::OPCODE_PING, NULL,0);
	}
	bool WebSocketClient::sendPong() {
		return sendFrame(WebsocketFrame::OPCODE_PONG, NULL, 0);
	}
	bool WebSocketClient::sendBye() {
		return sendFrame(WebsocketFrame::OPCODE_BYE, NULL, 0);
	}
	
	
}
