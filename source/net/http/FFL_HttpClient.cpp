/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpClient.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  http客户端，保存连接和缓存的数据
*
*/

#include <net/http/FFL_HttpClient.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpParser.hpp>
#include <net/http/FFL_HttpRequestBuilder.hpp>
#include <net/http/FFL_HttpResponseBuilder.hpp>

namespace FFL {	
	HttpClient::HttpClient(TcpClient* client):mClient(client),
		mData(&(client->mSocket)){
	}
	HttpClient::~HttpClient() {
	}
	//
	//  读取一个请求
	//
	FFL::sp<HttpRequest> HttpClient::readRequest() {
		HttpParser parser;
		FFL::sp<HttpRequest> request = new HttpRequest(this);
		if (FFL_OK == parser.parseRequest(&mData,request.get())) {
			return request;
		}
		return NULL;
	}
	//
	//  发送一个请求
	//
	bool HttpClient::writeRequest(FFL::sp<HttpRequest>& request) {
		if (!request->writeHeader()) {
			return false;
		}		
		return true;
	}

	//
	//  读取一个应答
	//
	FFL::sp<HttpResponse> HttpClient::readResponse() {
		HttpParser parser;
		FFL::sp<HttpResponse> response = new HttpResponse(this);
		if (FFL_OK == parser.parseResponse(&mData, response.get())) {
			return response;
		}
		return NULL;
	}	
	//
	//  发送一个应答
	//
	bool HttpClient::writeResponse(FFL::sp<HttpResponse>& response) {
		if (!response->writeHeader()) {
			return false;
		}			

		return response->writeContent();
	}

	//
	//  读取内容，
	//
	bool HttpClient::read(char* data, int32_t requstSize, size_t* readed){
		//mData.pull()
		return true;
    }

	//
	//  写内容
	//
	bool HttpClient::write(const char* data, int32_t requstSize, size_t* writed) {
		return mClient->write((void*)data, requstSize, writed)==FFL_OK;
	}

	//
	//  关闭这个连接
	//
	void HttpClient::close() {
		mClient->close();
	}
}
