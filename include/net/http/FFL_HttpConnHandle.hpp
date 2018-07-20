/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpConnHandle.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/17
*  https://github.com/zhenfei2016/FFL-v2.git
*  http 处理句柄 
*
*/
#ifndef _FFL_HTTP_CONNECT_HANDLE_HPP_
#define _FFL_HTTP_CONNECT_HANDLE_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>

namespace FFL {
	class HttpRequest;
	class HttpResponse;
	class HttpConnect;

	//
	// 处理http请求句柄接口类
	//
	class HttpConnectHandler : public RefBase{
	public:
		HttpConnectHandler();
		virtual ~HttpConnectHandler();

		//
		//   网络请求处理回调
		//   conn:网络连接
		//   request:网络请求
		//
		virtual void onReceiveRequest(HttpConnect* conn, HttpRequest* request)=0;		
	};

	//
	//  处理网络api请求
	//
	class HttpApiHandler : public RefBase {
	public:
		HttpApiHandler();
		virtual ~HttpApiHandler();

		virtual void onHttpQuery(HttpConnect* conn, FFL::String& path, FFL::String& query)=0;
	};
}

#endif