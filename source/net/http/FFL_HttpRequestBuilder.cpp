/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpRequestBuilder.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/08
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  构造请求结构类
*/

#include <net/http/FFL_HttpRequestBuilder.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpClient.hpp>


namespace FFL {	
	HttpRequestBuilder::HttpRequestBuilder() {

	}
	HttpRequestBuilder::~HttpRequestBuilder(){
	}
	//
	//  构造一个请求
	//
	FFL::sp<HttpRequest> HttpRequestBuilder::createRequest(HttpClient* client) {
		FFL::sp<HttpRequest> request = new HttpRequest(client);
		return request;
	}
	
}