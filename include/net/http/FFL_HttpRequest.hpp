/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpRequest.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http请求
*/
#ifndef _FFL_HTTP_REQUEST_HPP_
#define _FFL_HTTP_REQUEST_HPP_

#include <FFL_Core.h>
#include <FFL_Ref.hpp>
#include <net/http/FFL_HttpHeader.hpp>
#include <net/http/FFL_HttpUrl.hpp>
#include <net/http/FFL_HttpClient.hpp>
#include <net/FFL_NetStream.hpp>

namespace FFL {
	class HttpResponse;
	class HttpResponseBuilder;
	class HttpRequestBuilder;

	class FFLIB_API_IMPORT_EXPORT HttpRequest : public RefBase {
	protected:
		friend class HttpClient;
		friend class HttpParserImpl;
		friend class HttpResponseBuilder;
		friend class HttpRequestBuilder;

		HttpRequest();
		HttpRequest(FFL::sp<HttpClient> client);
	public:
		virtual ~HttpRequest();	
		FFL::sp<HttpResponse> makeResponse();
	public:	
		//
		//  指定client连接，发送接收的数据是在这一条连接上执行的 
		//
		void setHttpClient(FFL::sp<HttpClient> client);
		FFL::sp<HttpClient> getHttpClient();
		//
		//  请求参数相关
		//				
		void getUrl(HttpUrl& url);
		void setUrl(HttpUrl& url);		
		//
		//  头信息
		//
		void getHeader(HttpHeader& header);
		void setHeader(HttpHeader& header);
		//
		//  设置内容，用于发送端,如果数据量小可以直接设置data,size
		//  数据量大的情况下需要设置ContentStream指针
		//
		void setContent(const uint8_t* data, uint32_t size);
		void setContent(FFL::sp<HttpContent> content);
		//
		//  开始发送请求
		//  执行了写头，写内容操作 writeHeader();writeContent();
		//
		bool send();
	public:
		//
		//  请求内容
		//  header :头内容		
		//  content:内容
		//
		virtual bool writeHeader();
		virtual bool writeContent();
		//
		//  结束应答,关闭这个连接
		//
		virtual void finish();

		//
		//  读取内容,正常情况下当这个request parse完成后头信息已经读取完成
		//
		bool readContent(uint8_t* content, int32_t requestSize, size_t* readed);
	protected:
		//
		//  请求头
		//
		HttpHeader mHeader;	
		// parse uri to schema/server:port/path?query
		HttpUrl mUrl;	
		//
		//  发送的内容
		//
		FFL::sp<HttpContent> mContent;
		//
		//  那个连接上的请求
		//
		FFL::sp<HttpClient> mClient;
	};
}

#endif