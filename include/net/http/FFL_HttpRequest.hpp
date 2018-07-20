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
#ifndef _FFL_NET_REQUEST_HTTP_HPP_
#define _FFL_NET_REQUEST_HTTP_HPP_


#include <FFL.h>
#include <ref/FFL_Ref.hpp>
#include <net/http/FFL_HttpHeader.hpp>
#include <net/http/FFL_HttpUrl.hpp>
#include <net/FFL_NetStream.hpp>


namespace FFL {
	class HttpConnect;
	class HttpResponse;
	class HttpRequest : public RefBase {
		friend class HttpParserImpl;
	public:
		HttpRequest(HttpConnect* conn);
		virtual ~HttpRequest();
		//
		// 重置这个请求
		//
		void reset();
		//
		// 获取content类型
		//
		void getContentType(String& type) const;
		//
		// 获取content的长度
		//
		int32_t getContentLength() const;
		//
		//  是否Keep_Alive
		//
		bool isKeepAlive() const;

		const HttpUrl& getUrl() const {
			return mUrl;
		} 

		void getPath(String& path);
		void getQuery(String& query);
	public:
		//
		//  创建应答
		//
		sp<HttpResponse>  createResponse();
	private:
		//
		//  填充一下数据
		//
		void fill(const String& url,NetStreamReader* reader);
	private:
		//
		//  请求头
		//
		NetHttpHeader mHeader;
		String mContentType;
		int32_t mContentLength;
		bool mKeepAlive;

		// parse uri to schema/server:port/path?query
		HttpUrl mUrl;	

		//
		//  读取网络流接口
		//
		NetStreamReader* mReader;
		//
		//  那个连接上的请求
		//
		HttpConnect* mConn;
	};
}

#endif