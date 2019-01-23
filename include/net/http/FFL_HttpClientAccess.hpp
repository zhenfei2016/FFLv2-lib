/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpClientManager.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/15
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  http请求 sender  ,用于请求HttpRequest
*
*/

#ifndef _FFL_HTTP_CLIENT_ACCESS_MANGER_HPP_
#define _FFL_HTTP_CLIENT_ACCESS_MANGER_HPP_

#include <net/FFL_TcpClient.hpp>
#include <net/FFL_NetStream.hpp>
#include <ref/FFL_RefBase.hpp>
#include <net/FFL_NetEventLoop.hpp>
#include <utils/FFL_BlockList.hpp>

namespace FFL {   
   class HttpRequest;
   class HttpResponse;  
   class HttpClient;
   class HttpRequestThread;

   class FFLIB_API_IMPORT_EXPORT HttpClientAccessManager : public NetEventLoop::Callback{
	   friend class HttpRequestBuilder;
   public:
	   class Callback : public RefBase{
	   public:
		   enum {
			   //
			   //  没有错误
			   //
			   ERROR_SUC=0,
			   //
			   //  连接服务器失败
			   //
			   ERROR_CONNECT=-1,

			   ERROR_UNKONW =-2,
		   };
	   public:
		   //
		   //  网络应答
		   //  errorNo :错误码
		   //
		   virtual void onResponse(FFL::sp<HttpResponse> response,int32_t errorNo) = 0;
	   };
    public:
		HttpClientAccessManager();
		~HttpClientAccessManager();
   public:
		//
		//  启动，停止
	    //
		bool start();
		void stop();
		//
		//  发送一个请求
		//
		bool post(FFL::sp<HttpRequest>& request,FFL::sp<Callback> callback);
   protected:
	   /*  NetEventLoop::Callback  */
	   //
	   //  返回是否还继续读写
	   //  priv:透传数据
	   //
	   virtual bool onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv) ;
   protected:
	   //
	   //  请求队列
	   //
	   class RequestContext : public RefBase {
	   public:
		   RequestContext();
		   ~RequestContext();

		   int64_t mCreateTimeUs;
		   FFL::sp<HttpRequest> mRequest;
		   FFL::sp<Callback> mCallback;		   	   

		   TcpClient* mTcpClient;
		   FFL::sp<HttpClient> mHttpClient;
	   };
	   FFL::BlockingList< FFL::sp<RequestContext> > mRequestQueue;

	   //
	   //  挂起，等待应答的请求列表
	   //
	   FFL::CMutex mPendingLock;
	   std::list<  FFL::sp<RequestContext> > mPendingRequstList;
   protected:
	   //
	   //  处理这个请求
	   //
	   bool processRequest(FFL::sp<RequestContext> request);
   protected:
	   //
	   //  处理网络消息
	   //
	   NetEventLoop mEventLoop;	   

	   //
	   //  请求线程 
	   //
	   friend class HttpRequestThread;
	   FFL::sp<HttpRequestThread> mHttpRequestThread;
	
   };  
}
#endif
