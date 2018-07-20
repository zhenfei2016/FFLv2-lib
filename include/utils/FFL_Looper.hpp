/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Looper
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  参考 Android Open Source Project 进行修改的
*  消息循环类
*
*/


#ifndef _FFL_LOOPER_HPP_
#define _FFL_LOOPER_HPP_

#include <utils/FFL_Utils.hpp>
#include <utils/FFL_Clock.hpp>
#include <thread/FFL_Thread.hpp>

namespace FFL {

	class Handler;
	class IdleHandler;
	class Message;
	class Looper;	
	class HandlerManager;
	class MessageQueue;

	class Looper : public RefBase 
	{
	public:
		typedef int32_t event_id;
		typedef int32_t handler_id;

		Looper();		
		Looper(sp<Clock> clock);
		
		//
		//  设置looper名称
		//
		void setName(const char *name);
		
		//
		// 注册一个句柄，处理这个looper的消息
		// handler_id： 返回句柄id，
		// 只要构造的消息的 target指向这个id，则就会由这个handler进行处理
		//
		handler_id registerHandler(const sp<Handler> &handler);
		void unregisterHandler(handler_id handlerID);
		//
		//  注册一下空闲处理handler
		//
		void registerIdleHandler(sp<IdleHandler> handler);
		void unregisterIdleHandler();

		//
		//启动，停止这个Looper的处理流程
		//
		status_t start(int32_t priority = FFL_THREAD_PRIORITY_NORMAL);
		//
		//  请求并且等待退出
		//
		status_t stop();
		//
		//  请求退出，等待退出
		//
		status_t requestStop();
		status_t waitStop();
		//
		//  等待到looping，返回是否成功了
		//  waitMs:等待的时长，如果<0 则一直等待
		//
		bool waitLooping(int32_t waitMs);
		bool isLooping() const;

		//
		//  获取当前使用的messagelist
		//
		void getMessageList(List< sp<Message> >& list);
        //
        // 清空等待处理的消息队列
        //
		void clearMessageList(List< sp<Message> >* list);
		//
		// 清空等待处理的特定handler消息队列
		//
		void clearMessageList(List< sp<Message> >* list,Looper::handler_id handlerId);
        //
        // 取消一条消息
		//  msgId :消息的唯一id 
        //
        bool cancelMessage(uint32_t msgId);
		//
		// 获取这个looper的messagequeue
		//
		MessageQueue* getMessageQueue();
	public:
		void setDebug(bool debug);
	protected:
		virtual ~Looper();
	private:
		FFL::String mName;
		CMutex mLock;
			
		class LooperThread;
		sp<LooperThread> mThread;		
		//
		//  空闲处理句柄
		//
		wp<IdleHandler> mIdleHandler;
		bool mHaveIdleHandler;

		//
		//  保存这个looper下的所有handler
		//
		HandlerManager* mHandlerManager;
		//
		//  消息队列
		//
		MessageQueue* mMsgQueue;
	protected:
		//
        // 入队一条消息 , msg不可以为空
		// delayUs<0则会插入到最先执行的位置
		//		
		bool post(const sp<Message> &msg, int64_t delayUs);
		//
		// 发送退出looper消息
		//
		//bool postQuitMessage(int32_t exitCode);
		//
		// 发送刷新消息队列消息,
		// 清空消息队列，不管是否已经到达时间了
		//
		bool postFlushMessage();
		//
		//  消息循环
		//		
		bool loop();
	private:
		//
		//  退出looper
		//
		void onQuitLooper(const sp<Message> &msg);
			
	protected:
		DISABLE_COPY_CONSTRUCTORS(Looper);
	private:
		sp<Message> mMsgQuitLooper;
		sp<Message> mMsgFlush;		

	private:
		void setIsLooping(bool looping);
		CMutex mLoopingMutex;
		CCondition mLoopingCond;
		volatile int32_t mLooping;
	};
}  
#endif

