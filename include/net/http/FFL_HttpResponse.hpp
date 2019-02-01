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
#ifndef _FFL_HTTP_RESPONSE_HTTP_HPP_
#define _FFL_HTTP_RESPONSE_HTTP_HPP_


#include <FFL_Core.h>
#include <FFL_Ref.hpp>
#include <net/http/FFL_HttpHeader.hpp>
#include <net/http/FFL_HttpStreamCallback.hpp>

namespace FFL {
	class HttpClient;
	class ByteBuffer;	
	class FFLIB_API_IMPORT_EXPORT HttpResponse : public RefBase{
	protected:
	    friend class HttpClient;
		friend class HttpRequest;
		friend class HttpParserImpl;
		friend class HttpResponseBuilder;
		HttpResponse(FFL::sp<HttpClient> client);
	public:
		virtual ~HttpResponse();			
	public:
		//
		//  获取，设置 状态码
		//				
		int32_t getStatusCode();
		void setStatusCode(int32_t code);
		//
		//  获取，设置 头信息
		//
		void getHeader(HttpHeader& header);
		void setHeader(HttpHeader& header);
		//
		//  设置内容，用于发送,如果内容比较短，可以使用这个的
		//
		void setContent(const char* data,size_t size);
		//
		//  开始发送应答，
		//
		bool send();
	public:
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  发送，接收高级模式
	protected:
		//
		//  开始发送请求
		//  callback:如果发送大批量的数据的回调
		//  dataSize: 数据流总的大小
		//				
		bool sendStream(HttpStreamCallback* callback);
		bool recvStream(HttpStreamCallback* callback);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	protected:
		//
		//  读取内容,正常情况下当这个request parse完成后头信息已经读取完成
		//
		bool readContent(char* content, int32_t requestSize, size_t* readed);
	protected:
		//
		//  请求内容
		//  header :头内容		
		//  content:内容
		//
		bool writeHeader();		
		virtual bool writeContent();
		bool writeContent(const char* content, int32_t requestSize);
		//
		//  结束应答,关闭这个连接
		
		void finish();
	protected:
		//
		//  请求头
		//
		HttpHeader mHeader;
		int32_t mStatusCode;
		//
		//  保存写的内容
		//
		ByteBuffer* mContentBuffer;
		//
		//  那个连接上的请求
		//
		FFL::sp<HttpClient> mClient;
	};	

	
}

#endif