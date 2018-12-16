/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpRequestBuilder.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/08
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  ��������ṹ��
*/

#ifndef _FFL_HTTP_REQUEST_BUILD_HPP_
#define _FFL_HTTP_REQUEST_BUILD_HPP_

#include <ref/FFL_Ref.hpp>

namespace FFL {	
	class HttpRequest;
	class HttpResponse;
	class HttpClient;

	class HttpRequestBuilder {
	public:
		HttpRequestBuilder();
		~HttpRequestBuilder();

		//
		//  ����һ������
		//
		static FFL::sp<HttpRequest> createRequest(HttpClient* client);
	};
	
}

#endif