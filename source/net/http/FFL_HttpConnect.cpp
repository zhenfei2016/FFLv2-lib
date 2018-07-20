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

#include <net/http/FFL_HttpConnect.hpp>
#include <net/http/FFL_HttpConnManager.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpParser.hpp>
#include <net/FFL_NetStream.hpp>

namespace FFL {
	HttpConnect::HttpConnect(NetFD fd, sp<HttpConnectHandler> handler) :NetConnect(fd){
		mHandler = handler;
		mClosed = false;

		mSocket=new CSocket(fd);
		mStreamReader = new NetStreamReader(mSocket);
	}
	HttpConnect::~HttpConnect() {
		FFL_SafeFree(mSocket);
		FFL_SafeFree(mStreamReader);
	}	
	//
	//  处理fd上数据的循环，返回是否继续处理
	//
	bool HttpConnect::process() {
		mClosed = false;
		HttpParser parser;
		sp<HttpRequest> request=new HttpRequest(this);
		if (FFL_OK==parser.parseRequest(mStreamReader, request.get())) {
			sp<HttpConnectHandler> handler = mHandler.promote();
			if (!handler.isEmpty()) {
				handler->onReceiveRequest(this, request.get());
			}
		}else {
			getConnectManager()->destroyConnect(this->getFd());
			return false;
		}

		if (isClosed() ||!request->isKeepAlive()) {
			return false;
		}


		return true;
	}
	//
	//  获取读写接口
	//
	IOReader* HttpConnect::getReader() {
		return mSocket;
	}
	IOWriter* HttpConnect::getWriter() {
		return mSocket;
	}

	void HttpConnect::close() {
		mClosed = true;
	}

	bool HttpConnect::isClosed() {
		return mClosed;
	}
}