/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponse.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http应答
*/

#include <net/http/FFL_HttpResponse.hpp>
#include <utils/FFL_StringHelper.hpp>
#include <utils/FFL_ByteBuffer.hpp>
#include <utils/FFL_ByteStream.hpp>
#include <net/http/FFL_HttpClient.hpp>

namespace FFL {
	//const char* JSON_HEADER = "HTTP/1.1 200 OK \r\n"
	//	"Content-Type: application/json;charset=utf-8\r\n";
	HttpResponse::HttpResponse(FFL::sp<HttpClient> client): mContentBuffer(NULL){
		mClient=client;
		mStatusCode = 200;		
	}
	HttpResponse::~HttpResponse(){
	}	
	//
	//  状态码
	//				
	int32_t HttpResponse::getStatusCode() {
		return mStatusCode;
	}
	void HttpResponse::setStatusCode(int32_t code) {
		mStatusCode = code;
	}
	void HttpResponse::getHeader(HttpHeader& header) {
		header = mHeader;
	}
	void HttpResponse::setHeader(HttpHeader& header) {
		mHeader = header;
	}
	//
	//  设置内容，用于写
	//
	void HttpResponse::setContent(const char* data, size_t size) {
		mHeader.setContentLength(size);
		if (!mContentBuffer) {
			mContentBuffer = new ByteBuffer(size);
		}
		mContentBuffer->getByteStream()->writeBytes( (const int8_t*)data, size);
	}
	//
	//  开始发送应答，
	//  callback:如果发送大批量的数据的回调
	//  dataSize: 数据流总的大小
	//
	bool HttpResponse::send() {
		FFL::sp<HttpResponse> res=this;
		return mClient->writeResponse(res);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  发送，接收高级模式
	//
	//
	//  开始发送请求
	//  callback:如果发送大批量的数据的回调
	//  dataSize: 数据流总的大小
	//				
	bool HttpResponse::sendStream(HttpStreamCallback* callback) {
		if (callback == NULL) {
			return false;
		}
		char buf[4096] = {};
		size_t size = 0;
		while (true) {
			if (!callback->onReadData(buf, 4096, &size)) {
				break;				
			}

			if (size == 0) {
				break;
			}

			if (!writeContent(buf, size)) {
				return false;
			}
		}
		return true;
	}
	bool HttpResponse::recvStream(HttpStreamCallback* callback) {
		if (callback == NULL) {
			return false;
		}
		char buf[4096] = {};
		size_t size = 0;
		while (true) {
			if (!readContent(buf,4096, &size)) {
				return false;
			}

			if (!callback->onWriteData(buf, 4096)) {
				break;
			}			
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  读取内容
	//
	bool HttpResponse::readContent(char* content, int32_t requestSize, size_t* readed) {
		return mClient->read(content, requestSize, readed);
	}	
	//
	//  请求内容
	//  header :头内容		
	//  content:内容
	//
	bool HttpResponse::writeHeader() {
		//
		//  发送的内容长度
		//
		int32_t contentLength = mHeader.getContentLength();		
		if (contentLength < 0) {
			contentLength = 0;
		}

		String headerInfo;
		{
			String format;
			format = "HTTP/1.1 %d OK \r\n"
				"%s: %s\r\n"
				"%s: %d\r\n";
			String type;
			mHeader.getContentType(type);
			formatString(headerInfo, format.c_str(), mStatusCode,
				HTTP_KEY_CONTENTYPE,type.c_str(),
				HTTP_KEY_CONTENTLEN,contentLength);
		}

		std::map<String, String> keys;
		mHeader.getAll(keys);
		for (std::map<String, String>::iterator it = keys.begin(); it!= keys.end(); it++) {
			if (strcmp(HTTP_KEY_CONTENTYPE, it->first.c_str()) == 0) {
				continue;
			}
			else if (strcmp(HTTP_KEY_CONTENTLEN, it->first.c_str()) == 0) {
				continue;
			}

			headerInfo += it->first + ":" + it->second + "\r\n";
		}
		
		headerInfo += "\r\n";		
		return mClient->write(headerInfo.c_str(), headerInfo.size(),0);
	}
	bool HttpResponse::writeContent() {
		if (mContentBuffer) {
			const char* data =(const char*) (mContentBuffer->getByteStream()->getData());
			size_t dataLen = mContentBuffer->getByteStream()->getSize();
		    return writeContent(data, dataLen);
		}
		return false;			 
	}
	bool HttpResponse::writeContent(const char* content, int32_t requestSize) {
		return mClient->write(content, requestSize, 0);
	}
	//
	//  结束应答,关闭这个连接
	//
	void HttpResponse::finish() {
		mClient->close();
	}
}
