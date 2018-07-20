/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HandlerManager   
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
handler管理器，每一个looper中存在一个这个示例，用于handler的管理
*
*/
#ifndef _FFL_HANDLER_MANAGER_HPP_
#define _FFL_HANDLER_MANAGER_HPP_

#include <map>
#include <utils/FFL_Handler.hpp>

namespace FFL {


	class Handler;

	class HandlerManager 
	{
		friend class Looper;
	protected:
		HandlerManager(Looper* looper);
		~HandlerManager();
	public:
		//
		//  关联上handler和looper，并注册到系统中。
		//  注册成功后，对应hander_id的消息会得到这个handler的处理
		//  返回hanler_id
		//
		Looper::handler_id addHandler(const sp<Handler> &handler);
		//
		// 取消handlerID的handler
		//
		void removeHandler(Looper::handler_id handlerID);
		//
		// 取消一些无效的handler,可能外面已经把这个handler删除掉了
		//
		void removeStaleHandlers();
		void removeAllHandler();

		//
		// 获取对应的handler
		//
		sp<FFL::Handler> getHandler(Looper::handler_id handlerID);
		//
		// 获取所有的handler列表
		//
		void getAllHandler(List< sp<FFL::Handler> >& list);
		//
		//进行这个消息的处理，查找对应消息的handler进行处理
		//
		void deliverMessage(const sp<Message> &msg);
		//
		// 把消息投递给所有的handler
		//
		void deliverMessageToAllHandler(const sp<Message> &msg);
	protected:
		struct HandlerInfo
		{			
			wp<Handler> mHandler;
		};

		//
		// 所有的handler
		//
		CMutex mLock;
		typedef std::map<Looper::handler_id, HandlerInfo> KeyedVectorHandler;
		KeyedVectorHandler mHandlers;

		//
		//  这个mgr管理了哪一个looper的handler
		//
		Looper* mLooper;
	};
}  

#endif
