/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpRequest.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http请求
*/
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpClient.hpp>
#include <FFL_ByteBuffer.hpp>
#include <FFL_ByteStream.hpp>

namespace FFL {	
	class SimpleHttpContent : public HttpContent {
	public:
		SimpleHttpContent(const uint8_t* data,int32_t size) {
			mBuffer = new ByteBuffer(data,size);
		}

		~SimpleHttpContent() {
			FFL_SafeFree(mBuffer);
		}

		int32_t getSize() {
			return mBuffer->getByteStream()->getSize();
		}
		int32_t read(uint8_t* data, int32_t* bufSize) {
			int32_t size = mBuffer->getByteStream()->getSize();
			if (bufSize) {
				if (size > *bufSize) {
					size = *bufSize;
				} else {
					*bufSize = size;
				}
			}

			mBuffer->getByteStream()->readBytes((int8_t*)data, size);
			return size;
		}
		ByteBuffer* mBuffer;
	};
	HttpRequest::HttpRequest() {
	}
	HttpRequest::HttpRequest(FFL::sp<HttpClient> client):
		   mClient(client){
	}
	HttpRequest::~HttpRequest() {
	}
	void HttpRequest::setHttpClient(FFL::sp<HttpClient> client) {
		mClient = client;
	}
	FFL::sp<HttpClient> HttpRequest::getHttpClient() {
		return mClient;
	}

	FFL::sp<HttpResponse> HttpRequest::makeResponse() {
		FFL::sp<HttpResponse> response = new HttpResponse(mClient);
		return response;
	}
	//
	//  请求参数相关
	//				
	void HttpRequest::getUrl(HttpUrl& url) {
		url = mUrl;
	}
	void HttpRequest::setUrl(HttpUrl& url) {
		mUrl = url;
	}
	void HttpRequest::getHeader(HttpHeader& header) {
		header = mHeader;
	}
	void HttpRequest::setHeader(HttpHeader& header) {
		mHeader = header;
	}
	//
	//  设置内容，用于发送端
	//
	void HttpRequest::setContent(const uint8_t* data, uint32_t size) {
		if (data && size > 0) {
			mContent = new SimpleHttpContent(data,(int32_t) size);
		}
		else {
			mContent = NULL;
		}
	}
	void HttpRequest::setContent(FFL::sp<HttpContent> content) {
		mContent = content;
	}

	//
	//  开始发送请求，
	//
	bool HttpRequest::send() {
		if (writeHeader()) {
			return writeContent();
		}
		return false;		
	}	
	//
	//  请求内容
	//  header :头内容		
	//  content:内容
	//
	bool HttpRequest::writeHeader() {
		if (mClient.isEmpty()) {
			return false;
		}

		//
		//  发送的内容长度
		//
		int32_t contentLength = mHeader.getContentLength();
		if (contentLength < 0) {
			contentLength = 0;
		}

		String headerInfo;
		{
			static const char* format =
				"POST %s HTTP/1.1\r\n"
				"%s: %s\r\n"
				"%s: %d\r\n";

			String type;
			mHeader.getContentType(type);

			headerInfo = String::format(format, 
				(mUrl.mPath + "?" + mUrl.mQuery).string(),
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
	}
	bool HttpRequest::writeContent() {
		if (mClient.isEmpty()) {
			return false;
		}

		if (mContent.isEmpty()) {
			return true;
		}

		uint8_t buf[4096] = {};
		int32_t bufSize = 4096;
		int32_t len=mContent->getSize();
		while(len>0){
			bufSize = 4096;
			if (mContent->read(buf, &bufSize) <= 0) {
				break;
			}

			if (!mClient->write((const char*)buf, bufSize, 0)) {
				break;
			}

			FFL_sleep(5);
		}
		
		return true;
	}
	//
	//  结束应答,关闭这个连接
	//
	void HttpRequest::finish() {
		if (!mClient.isEmpty()) {
			mClient->close();
			mClient = NULL;
		}

		mContent = NULL;
	}

	//
	//  读取内容
	//
	bool HttpRequest::readContent(uint8_t* content, int32_t requestSize, size_t* readed) {
		if (mClient.isEmpty()) {
			return false;
		}

		return mClient->read((char*)content, requestSize, readed);
	}
}

