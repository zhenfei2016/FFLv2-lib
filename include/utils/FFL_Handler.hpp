/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Handler
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  https://github.com/zhenfei2016/FFL-v2.git
*
参考 Android Open Source Project 进行修改的
*  处理消息的回调函数
*
*/

#ifndef _FFL_HANDLER_HPP_
#define _FFL_HANDLER_HPP_


#include <thread/FFL_Thread.hpp>
#include "FFL_Looper.hpp"

namespace FFL {

	class Handler;
	class Message;
	class Looper;

	class Handler : public RefBase
	{
	public:
		Handler();

		Looper::handler_id id() const;
	protected:
		virtual void onMessageReceived(const sp<Message> &msg) = 0;

	private:		
		friend class HandlerManager;
		friend class Looper;
		Looper::handler_id mID;
		inline void setID(Looper::handler_id id) 
		{
			mID = id;
		}

		DISABLE_COPY_CONSTRUCTORS(Handler);
	};

	//
	//  空闲时候的回调
	//
	class IdleHandler : public RefBase
	{
	public:
		IdleHandler();
		virtual ~IdleHandler();

		virtual void onIdle()=0;
	};
}  

#endif
