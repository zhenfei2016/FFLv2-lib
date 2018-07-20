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
#include <net/http/FFL_HttpConnect.hpp>

namespace FFL {
	HttpRequest::HttpRequest(HttpConnect* conn):
		mConn(conn){
		reset();
	}
	HttpRequest::~HttpRequest() {		
	}
	void HttpRequest::reset() {
		mReader = NULL;
		mContentType="";
		mContentLength = 0;
		mKeepAlive = false;
	}
	//
	// 获取content类型
	//
	void HttpRequest::getContentType(String& type) const {
		type = mContentType;
	}
	//
	// 获取content的长度
	//
	int32_t HttpRequest::getContentLength() const {
		return mContentLength;
	}
	//
	//  是否Keep_Alive
	//
	bool HttpRequest::isKeepAlive() const {
		return mKeepAlive;
	}
	//
	//  填充一下数据
	//
	void HttpRequest::fill(const String& url, NetStreamReader* reader) {
		mUrl.parse(url);

		mReader = reader;
		mHeader.getKey("Content Type", mContentType);
		mHeader.getKeyInt32("Content Length", mContentLength,0);

		int32_t value=0;
		mHeader.getKeyInt32("Keep-alive", value, 0);
		mKeepAlive = value != 0;
	}

	//
	//  创建应答
	//
	sp<HttpResponse>  HttpRequest::createResponse() {
		HttpResponse* response = new HttpResponse(mConn);
		return response;
	}
	void HttpRequest::getPath(String& path) {
		path=mUrl.mPath;
	}
	void HttpRequest::getQuery(String& query) {
		query = mUrl.mQuery;
	}
}

