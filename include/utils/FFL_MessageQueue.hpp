/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_MessageQueue.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/11/26
*  https://github.com/zhenfei2016/FFL-v2.git
*  保存消息的消息队列
*
*/
#ifndef _FFL_MESSAGE_QUEUE_HPP_
#define _FFL_MESSAGE_QUEUE_HPP_

#include <utils/FFL_List.hpp>
#include <utils/FFL_Clock.hpp>
#include <utils/FFL_Message.hpp>
#include <thread/FFL_Mutex.hpp>

namespace FFL {
	class MessageQueue : public IClockListener {
	public:
		MessageQueue(sp<Clock> clock);
		virtual ~MessageQueue();
		//
		// 当前队列的大小
		//
		uint32_t size();
        //
        //  插入一条消息msg不可以为空,返回插入的索引位置，最靠前的最先处理
        //  消息在 createTime+ delayUs 时刻执行
		//  delayUs<0则会插入到最先执行的位置
        //
        int32_t post(const sp<Message> &msg,int64_t createTime,int64_t delayUs);
		//
		//  插入一条消息msg不可以为空,返回插入的索引位置，最靠前的最先处理
		//  delayUs<0则会插入到最先执行的位置
		//
		int32_t post(const sp<Message> &msg, int64_t delayUs);
		//
		//  如果当前正在next中的等待状态，则wakeup会触发nextBlock的返回
		//
		void wakeup();
		//
		//  提取出下一条消息,如果不存在则返回空。不弹出队列
		//
		sp<Message> peek();
		//
		//  提取出下一条消息,并且弹出队列，如果没有消息了则会进行等待
		//  等待状态下wakeup可以激活它
		//
		sp<Message> nextBlock();
        //
        //  不驻塞模式下获取一条消息，没有则返回null
        //
        sp<Message> next();
		//
		//  获取当前使用的messagelist,不弹出队列
		//
		void peekAll(List< sp<Message> >* list);
		//
		//  获取当前使用的messagelist，弹出队列
		//
		void getAll(List< sp<Message> >* list);
		//
		//  获取当前handleId使用的messagelist，弹出队列
		//
		void getAll(List< sp<Message> >* list, Looper::handler_id handleId);
		//
		//  获取当前handleId使用的messagelist，弹出队列
		//
		//void getAllEx(List< sp<Message> >* list, Looper::handler_id handleId);
		//
		//  取消一条消息
		//  msgId:消息的唯一ID
		//
		bool cancel(uint32_t msgId);
	private:
		
		//  IClockListener 
		//  type : 速度增加，不变，减少 1，0，-1
		//  speed :当前的速度
		//
		virtual void onSpeedChange(int32_t type, int32_t speed);
	private:
		CMutex mLock;
		CCondition mQueueChangedCondition;
		struct MessageEntry
		{
            //
            //  消息的创建时间，和在创建时间基础上什么时候显示
            //
            int64_t mWorldCreatetimeUs;
            int64_t mWorldDelayTimeUs;
            //
            //  当前mClock时钟下的创建时间
            //
            int64_t mClockCreateTimeUs;
			sp<Message> mMessage;
		};
		//
		// 消息队列
		//
		List<MessageEntry> mMsgQueue;

		int32_t mRequestWakeup;
		//
		//  消息出队依靠这个时钟计算时间点
		//
		sp<Clock> mClock;

	public:
		//
		//  调试状态
		//
		bool mDebug;
	};
}
#endif
