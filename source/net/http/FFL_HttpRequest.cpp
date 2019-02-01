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

namespace FFL {	
	HttpRequest::HttpRequest(FFL::sp<HttpClient> client):mClient(client)
		{
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
	//  开始发送请求，
	//
	bool HttpRequest::send() {
		return true;
	}	
	//
	//  读取内容
	//
	bool HttpRequest::readContent(char* content, int32_t requestSize, size_t* readed) {
		return mClient->read(content, requestSize, readed);
	}
	
	//
	//  请求内容
	//  header :头内容		
	//  content:内容
	//
	bool HttpRequest::writeHeader() {
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
	bool HttpRequest::writeContent(const char* content, int32_t requestSize) {
		return mClient->write(content, requestSize,0);
	}
	//
	//  结束应答,关闭这个连接
	//
	void HttpRequest::finish() {
		mClient->close();
	}
}

