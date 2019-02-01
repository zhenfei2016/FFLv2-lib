/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponseFile.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/12/08
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http应答文件请求的
*/
#ifndef _FFL_HTTP_RESPONSE_BUILDER_HPP_
#define _FFL_HTTP_RESPONSE_BUILDER_HPP_

#include <FFL_Ref.hpp>

namespace FFL {	
	class HttpRequest;	
	class HttpResponse;
	class HttpResponseFile;

	class FFLIB_API_IMPORT_EXPORT HttpResponseBuilder {
	public:
		HttpResponseBuilder();
		~HttpResponseBuilder();
	public:
		//
		//  构造一个应答
		//
		FFL::sp<HttpResponse> createResponse(HttpRequest* request);
		//
		//  构造一个文件应答
		//
		FFL::sp<HttpResponseFile> createResponseFile(HttpRequest* request);
	};
}

#endif