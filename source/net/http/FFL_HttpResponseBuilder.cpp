/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponseFile.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/08
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http应答文件请求的
*/

#include <net/http/FFL_HttpResponseBuilder.hpp>

#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpResponseFile.hpp>
#include <net/http/FFL_HttpClient.hpp>

namespace FFL {	

	
	HttpResponseBuilder::HttpResponseBuilder() {

	}
	HttpResponseBuilder::~HttpResponseBuilder() {

	}
	//
	//  构造一个应答
	//
	FFL::sp<HttpResponse> HttpResponseBuilder::createResponse(HttpRequest* request) {
		FFL::sp<HttpResponse> response = new HttpResponse(request->mClient);
		return response;
	}
	//
	//  构造一个文件应答
	//
	FFL::sp<HttpResponseFile> HttpResponseBuilder::createResponseFile(HttpRequest* request) {
		FFL::sp<HttpResponseFile> response = new HttpResponseFile(request->mClient);
		return response;
	}
	

}