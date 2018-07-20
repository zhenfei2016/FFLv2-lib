/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponse.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http应答
*/

#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpConnect.hpp>

namespace FFL {
	const char* JSON_HEADER = "HTTP/1.1 200 OK \r\n"
		"Content-Type: application/json;charset=utf-8\r\n";

	HttpResponse::HttpResponse(HttpConnect* conn):mConn(conn){
	}
	HttpResponse::~HttpResponse() {

	}

	//
	//  设置返回的状态码
	//
	void HttpResponse::setStatusCode(int32_t code) {
		mStatusCode = code;
	}	

	bool HttpResponse::writeHeader(HttpHeader* header, const char* content) {
		return true;
	}

	bool HttpResponse::writeJson(const String& json) {
		String format;
		format = "HTTP/1.1 200 OK \r\n"
			"Content-Type: application/json;charset=utf-8\r\n"
			"Content-Length:%d\r\n\r\n%s";

		String content;
		formatString(content, format.c_str(), json.size(), json.c_str());


		FFL::IOWriter* writer = mConn->getWriter();


		size_t nWrited = 0;
		writer->write((void*)content.c_str(), content.size(), &nWrited);
		return nWrited>0;
	}
	//
	//  创建应答
	//
	sp<HttpRequest>  HttpResponse::createRequest() {
		HttpRequest* req = new HttpRequest(mConn);
		return req;
	}

}
