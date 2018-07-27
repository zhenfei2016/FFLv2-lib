/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponse.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http应答
*/
#ifndef _FFL_NET_RESPONSE_HTTP_HPP_
#define _FFL_NET_RESPONSE_HTTP_HPP_


#include <FFL.h>
#include <ref/FFL_Ref.hpp>
#include <net/http/FFL_HttpHeader.hpp>

namespace FFL {
	class HttpConnect;
	class HttpRequest;
	class File;
	class HttpResponse : public RefBase{
	public:
		HttpResponse(HttpConnect* conn);
		virtual ~HttpResponse();

		//
		//  设置返回的状态码
		//
		void setStatusCode(int32_t code);

		bool writeHeader(HttpHeader* header,const char* content);
		bool writeJson(const String& json);
		void finish();

		sp<HttpRequest>  createRequest();
	protected:
		int32_t mStatusCode;

		HttpConnect* mConn;
	};

	class HttpHtmlResponse : public HttpResponse {
	public:
		HttpHtmlResponse(HttpConnect* conn);
		virtual ~HttpHtmlResponse();

		void htmlPath(const char* path);
		void response();
	private:
		FFL::File* mHtmlFile;
	};

	class HttpFileResponse : public HttpResponse {
	public:
		HttpFileResponse(HttpConnect* conn);
		virtual ~HttpFileResponse();
		
		//
		//  应答这个文件，返回是否有效的应答了
		//
		bool response(const char* file);
	private:
		
	};
}

#endif