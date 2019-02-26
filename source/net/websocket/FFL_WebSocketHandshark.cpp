/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_WebSocketHandshark.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  握手
*/
#include "FFL_WebSocketHandshark.hpp"
#include "FFL_Base64.hpp"
#include "FFL_Sha1.hpp"

namespace FFL {	
	static const char* kWebSocketSalt = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	/////////////////////////////////////////////////////////////////////////
	WSHandsharkRequest::WSHandsharkRequest(FFL::sp<HttpClient>& client):HttpRequest(client) {
		mMethod = GET;
		mVersion = 13;
	}	
	WSHandsharkRequest::~WSHandsharkRequest() {
	}
	//
	//  获取设置版本号
	//
	void WSHandsharkRequest::setSecVersion(int32_t version) {
		mVersion = version;
	}
	int32_t WSHandsharkRequest::getSecVersion() {
		return mVersion;
	}
	//
	//  获取设置key
	//
	void WSHandsharkRequest::setSecKey(String& key) {
		mKey = key;
	}
	bool WSHandsharkRequest::getSecKey(String& key) {
		key = mKey;
		return true;
	}
	//
	//  请求内容
	//  header :头内容		
	//  content:内容
	//
	//  GET / HTTP / 1.1
	//	Connection:Upgrade
	//	Host : 127.0.0.1 : 8088
	//	Origin : null
	//	Sec - WebSocket - Extensions : x - webkit - deflate - frame
	//	Sec - WebSocket - Key : puVOuWb7rel6z2AVZBKnfw ==
	//	Sec - WebSocket - Version : 13
	//	Upgrade : websocket

	bool WSHandsharkRequest::writeHeader() {
		if (mClient.isEmpty()) {
			return false;
		}
			
		//
		//Connection:Connection必须设置为Upgrade，表示客户端希望连接升级
		//Upgrade : Upgrade必须设置为WebSocket，表示在取得服务器响应之后，使用HTTP升级将HTTP协议转换(升级)为WebSocket协议。
		//Sec - WebSocket - key : 随机16字节字符串进行base64，用于验证协议是否为WebSocket协议而非HTTP协议
		//Sec - WebSocket - Version : 表示使用WebSocket的哪一个版本。
		//
		String headerInfo;
		{
			static const char* format =
				"%s %s HTTP/1.1\r\n"
				"Connection: Upgrade\r\n"
				"Upgrade: websocket\r\n"				
				"Sec-WebSocket-key: %s\r\n"
				"Sec-WebSocket-Version: 13\r\n"
				"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\n"
				"Sec-WebSocket-Extensions: permessage - deflate; client_max_window_bits\r\n";
				
			String type;
			mHeader.getContentType(type);

			String path;
			if (mUrl.mPath.isEmpty()){
				path = "/";
			}
			else {
				path = mUrl.mPath + "?" + mUrl.mQuery;
			}

			headerInfo = String::format(format,
				"GET",
				path.string(),
				"hj0eNqbhE/A0GkBXDRrYYw==" );
		}

		int32_t buffCount = 20;
		FFL::Dictionary::Pair pairs[20];
		mHeader.getAll(pairs, &buffCount);

		for (int32_t i = 0; i < buffCount; i++) {
			if (strcmp(HTTP_KEY_CONTENTYPE, pairs[i].key.string()) == 0) {
				continue;
			}
			else if (strcmp(HTTP_KEY_CONTENTLEN, pairs[i].key.string()) == 0) {
				continue;
			}
			headerInfo += pairs[i].key + ":" + pairs[i].value + "\r\n";
		}

		headerInfo += "\r\n";
		return mClient->write(headerInfo.string(), headerInfo.size(), 0);
	}	

	WSHandsharkResponse::WSHandsharkResponse(FFL::sp<HttpClient>& client,String& requestKey) :
		HttpResponse(client) {
		calcSecAccess(requestKey);
	}
	//
	//  接收端应答创建函数
	//
	WSHandsharkResponse::WSHandsharkResponse(FFL::sp<HttpClient>& client):HttpResponse(client) {

	}

	WSHandsharkResponse::~WSHandsharkResponse() {

	}

	//
	//  获取服务端计算的key
	//		
	bool WSHandsharkResponse::getSecAccess(String& key) {
		key = mSecAccess;
		return !key.isEmpty();
	}
	//
	//  应答内容
	//  header :头内容		
	//  content:内容
	//

	//HTTP / 1.1 101 Switching Protocols
	//	Connection : Upgrade
	//	Server : beetle websocket server
	//	Upgrade : WebSocket
	//	Date : Mon, 26 Nov 2012 23 : 42 : 44 GMT
	//	Access - Control - Allow - Credentials : true
	//	Access - Control - Allow - Headers : content - type
	//	Sec - WebSocket - Accept : FCKgUr8c7OsDsLFeJTWrJw6WO8Q =
	bool WSHandsharkResponse::writeHeader() {
		//
		//  发送的内容长度
		//
		int32_t contentLength = 0;
		if (!mContent.isEmpty()) {
			contentLength = mContent->getSize();
		}
		else {
			contentLength = mHeader.getContentLength();
		}
		if (contentLength < 0) {
			contentLength = 0;
		}

		String headerInfo;
		{
			static const char* kFormat = "HTTP/1.1 101 Switching Protocols \r\n"
				"Connection: Upgrade\r\n"
				"Server: FFL-websocket\r\n"
				"Upgrade: WebSocket\r\n"
				"Access-Control-Allow-Credentials: true\r\n"
				"Access-Control-Allow-Headers: content-type\r\n"
				"Sec-WebSocket-Accept: %s\r\n";

			String type;
			mHeader.getContentType(type);
			headerInfo.appendFormat(kFormat, mStatusCode,
				HTTP_KEY_CONTENTYPE, type.string(),
				HTTP_KEY_CONTENTLEN, contentLength);
		}

		int32_t buffCount = 20;
		FFL::Dictionary::Pair pairs[20];
		mHeader.getAll(pairs, &buffCount);

		for (int32_t i = 0; i < buffCount; i++) {
			if (strcmp(HTTP_KEY_CONTENTYPE, pairs[i].key.string()) == 0) {
				continue;
			}
			else if (strcmp(HTTP_KEY_CONTENTLEN, pairs[i].key.string()) == 0) {
				continue;
			}
			headerInfo += pairs[i].key + ":" + pairs[i].value + "\r\n";
		}

		headerInfo += "\r\n";
		return mClient->write(headerInfo.string(), headerInfo.size(), 0);
		return false;
	}

	void WSHandsharkResponse::calcSecAccess(const String& requestkey) {
		String key = requestkey;
		key+= kWebSocketSalt;

		uint8_t shalHash[20] = {};
		SHA1_CTX context;
		FFL::SHA1Init(&context);
		FFL::SHA1Update(&context,(const uint8_t*) key.string(), key.length());
		FFL::SHA1Final(&context, shalHash);

		uint8_t base64Hash[32] = {};
		FFL_Base64Decode(shalHash, 20, base64Hash,31);

		mSecAccess =(const char*)base64Hash;
	}

	bool WSHandsharkResponse::isWebSocket(HttpResponse* response) {
		FFL::HttpHeader header;
		response->getHeader(header);

		String value;
		if (!header.getKey("Connection", value)||
			!value.equal("Upgrade")) {
			return false;
		}
		

		if (!header.getKey("Upgrade", value)||
			!value.equal("WebSocket")) {
			return false;
		}

		return true;
	}
	bool WSHandsharkResponse::getSec_WebSocket_Server(HttpResponse* response, String& value) {
		FFL::HttpHeader header;
		response->getHeader(header);		
		return header.getKey("Server", value);
	}
	bool WSHandsharkResponse::getSec_WebSocket_Accept(HttpResponse* response, String& key) {
		FFL::HttpHeader header;
		response->getHeader(header);
		return header.getKey("Sec-WebSocket-Accept", key);
	}

	bool WSHandsharkResponse::isHandsharkOk(WSHandsharkRequest* request, HttpResponse* response) {
		String requestkey;
		if (!request->getSecKey(requestkey)) {
			return false;
		}

		String accessKey;
		if (!getSec_WebSocket_Accept(response, accessKey)) {
			return false;
		}

		String key = requestkey;
		key += kWebSocketSalt;

		uint8_t shalHash[20] = {};
		SHA1_CTX context;
		FFL::SHA1Init(&context);
		FFL::SHA1Update(&context, (const uint8_t*)key.string(), key.length());
		FFL::SHA1Final(&context, shalHash);

		uint8_t base64Hash[32] = {};
		FFL_Base64Decode(shalHash, 20, base64Hash, 31);
			
		if (accessKey.equal((const char*)base64Hash)) {
			return true;
		}
		return false;
	}

}

