/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpConnect.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/15 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  一条http网络连接
*/
#ifndef _FFL_NETCONNECT_HTTP_HPP_
#define _FFL_NETCONNECT_HTTP_HPP_

#include <FFL.h>
#include <net/http/FFL_HttpConnHandle.hpp>
#include <net/FFL_NetConnect.hpp>
#include <net/FFL_NetStream.hpp>
#include <net/FFL_NetSocket.hpp>

namespace FFL {
    class HttpRequest;
    class HttpResponse;
	//
	//  http连接
	//
	class HttpConnect : public TcpConnect {
	public:
		HttpConnect(NetFD fd, sp<HttpConnectHandler> handler);
		~HttpConnect();
	public:
		//
		//  获取读写接口
		//
		IOReader* getReader();
		IOWriter* getWriter();

		void close();
		bool isClosed();
        
        sp<HttpResponse> createResponse();
        sp<HttpRequest> createRequest();		
	public:
		//
		//  处理fd上数据的循环，返回是否继续处理
		//
		virtual bool process();
	private:
		NetStreamReader* mStreamReader;
		wp<HttpConnectHandler> mHandler;

		CSocket* mSocket;
		bool mClosed;
	};
}

#endif
