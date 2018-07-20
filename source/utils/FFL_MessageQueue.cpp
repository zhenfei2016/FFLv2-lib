/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_MessageQueue.cpp 
 *  Created by zhufeifei(34008081@qq.com) on 2017/11/26 
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *  消息队列
 *
*/

#include <utils/FFL_MessageQueue.hpp>

namespace FFL {
	const char* TAG_MESSAGEQUEUE = "MsgQueue";

	MessageQueue::MessageQueue(sp<Clock> clock):mRequestWakeup(false){
		mDebug = false;
		if (clock.isEmpty()) {
			mClock = new Clock();
		}else {
			mClock = clock;
		}
		mClock->setListener(this);		
	}
	MessageQueue::~MessageQueue() {
		mClock->setListener(NULL);
	}
	//
	// 当前队列的大小
	//
	uint32_t MessageQueue::size() {
		CMutex::Autolock l(mLock);
		return mMsgQueue.size();
	}
    //
    //  插入一条消息msg不可以为空,返回插入的索引位置，最靠前的最先处理
    //  消息在 createTime+ delayUs 时刻执行
    //
    int32_t MessageQueue::post(const sp<Message> &msg,int64_t createTime,int64_t delayUs){
        if (msg.isEmpty()) {
            FFL_LOG_WARNING("failed to MessageQueue.post null message.\n");
            return -1;
        }
        
        //
        //  插入队列用系统的时钟
        //
        CMutex::Autolock l(mLock);
        int64_t whenUs=createTime +delayUs;
        //
        //比较时间 ,找到插入位置
        //最终最靠前的，执行的越早
        //
        int index = 0;
		List<MessageEntry>::iterator it = mMsgQueue.begin();
		if (delayUs >= 0) {
			for (; it != mMsgQueue.end() && ((*it).mWorldCreatetimeUs + (*it).mWorldDelayTimeUs) <= whenUs; it++) {
				index++;
			}
		}

        MessageEntry event;        
        event.mMessage = msg;        
        event.mWorldCreatetimeUs=createTime;
        event.mWorldDelayTimeUs=delayUs;
        event.mClockCreateTimeUs=mClock->worldToClockUs(createTime);		
        
		if (mDebug) {
			FFL_LOG_DEBUG_TAG(TAG_MESSAGEQUEUE,
				"MessageQueue::post worldCreateTime=%" lld64 " worldDelayUs=%" lld64 " whenUs=%" lld64 " clockCreate=%" lld64,
				createTime,
				delayUs, 
				whenUs,
				event.mClockCreateTimeUs);
		}

        if (index==0) {
            mQueueChangedCondition.signal();
        }
        
        mMsgQueue.insert(it, event);
        return index;

    }
	//
	//  插入一条消息msg不可以为空,返回插入的索引位置，最靠前的最先处理
	//
	int32_t MessageQueue::post(const sp<Message> &msg, int64_t delayUs) {
        return post(msg,FFL_getNowUs(), delayUs);
    }
	//
	//  如果当前正在next中的等待状态，则wakeup会触发next的返回
	//
	void MessageQueue::wakeup() {
		CMutex::Autolock l(mLock);
		mRequestWakeup = true;
		mQueueChangedCondition.signal();
	}
	//
	//  提取出下一条消息,如果不存在则返回空。不弹出队列
	//
	sp<Message> MessageQueue::peek() {
		CMutex::Autolock l(mLock);
		if (mMsgQueue.size() == 0) {
			return NULL;
		}

		MessageEntry msg = *mMsgQueue.begin();
		int64_t whenUs = msg.mWorldCreatetimeUs+ msg.mWorldDelayTimeUs;
		int64_t nowUs = mClock->nowUs();
		if (whenUs > nowUs) {			
			return NULL;
		}			
		return msg.mMessage;
	}
	//
	//  提取出下一条消息,并且弹出队列，如果没有消息了则会进行等待
	//  等待状态下wakeup可以激活它
	//
	sp<Message> MessageQueue::nextBlock() {
		sp<Message> findedMsg;
		CMutex::Autolock l(mLock);
		while (!mRequestWakeup){
			if (mMsgQueue.size() == 0) {
				mQueueChangedCondition.wait(mLock);
			}

			if (mMsgQueue.size() == 0) {
				continue;
			}

			MessageEntry msg = *mMsgQueue.begin();	
			int64_t worldUs = 0;
			int64_t nowUs  = mClock->nowUs(&worldUs);            
			int64_t whenUs = msg.mClockCreateTimeUs + msg.mWorldDelayTimeUs;
            
			if (whenUs <= nowUs) {
				//
				// 到需要处理的时间了
				//
				findedMsg = msg.mMessage;
				if (mDebug) {
					FFL_LOG_DEBUG_TAG(TAG_MESSAGEQUEUE,"MessageQueue::nextBlock worldCreateTime=%" lld64 " worldDelayUs=%" lld64 " whenUs=%" lld64 " clockCreate=%" lld64,
						msg.mWorldCreatetimeUs, 
						msg.mWorldDelayTimeUs,
						whenUs,
						msg.mClockCreateTimeUs);
				}		
				break;
			}else {
				int64_t delayUs = mClock->clockToWorldTimeBucket(whenUs- nowUs);
				mQueueChangedCondition.waitRelative(mLock, (uint32_t)(delayUs / 1000));
			}
		}

		if (mRequestWakeup) {
			mRequestWakeup = false;
		}

		if (!findedMsg.isEmpty()) {		
			mMsgQueue.erase(mMsgQueue.begin());
		}
		return findedMsg;
	}
    //
    //  不驻塞模式下获取一条消息，没有则返回null
    //
    sp<Message> MessageQueue::next() {
        CMutex::Autolock l(mLock);
        if (mMsgQueue.size() == 0) {
            return NULL;
        }
        
        MessageEntry msg = *mMsgQueue.begin();
		int64_t whenUs = msg.mClockCreateTimeUs + msg.mWorldDelayTimeUs;
        int64_t nowUs = mClock->nowUs();

        if (whenUs > nowUs) {
            return NULL;
        }
        mMsgQueue.erase(mMsgQueue.begin());

		if (mDebug) {
			FFL_LOG_DEBUG_TAG(TAG_MESSAGEQUEUE,"MessageQueue::next createTime=%" lld64 "delayUs=%" lld64 " whenUs=%" lld64 " clockNow=%" lld64,
				msg.mWorldCreatetimeUs, msg.mWorldDelayTimeUs, whenUs, nowUs);
		}
        return msg.mMessage;
    }
    
	//
	//  获取当前使用的messagelist,不弹出队列
	//
	void MessageQueue::peekAll(List< sp<Message> >* list) {
		CMutex::Autolock l(mLock);			
		if (list) {
			List<MessageEntry>::iterator it = mMsgQueue.begin();
			for (; it != mMsgQueue.end(); it++) {
				list->push_back(it->mMessage);
			}
		}
	}
	//
	//  获取当前使用的messagelist，弹出队列
	//
	void MessageQueue::getAll(List< sp<Message> >* list) {
		CMutex::Autolock l(mLock);
		if (list) {
			List<MessageEntry>::iterator it = mMsgQueue.begin();
			for (; it != mMsgQueue.end(); it++) {
				list->push_back(it->mMessage);
			}
		}
		mMsgQueue.clear();
	}
	//
	//  获取当前handleId使用的messagelist，弹出队列
	//
	void MessageQueue::getAll(List< sp<Message> >* list, Looper::handler_id handleId) {
		CMutex::Autolock l(mLock);
		List<MessageEntry>::iterator it = mMsgQueue.begin();
		for (; it != mMsgQueue.end(); ) {
			MessageEntry& entry = *it;
			if (entry.mMessage->target() == handleId) {
				if(list)
		   		  list->push_back(it->mMessage);
				it = mMsgQueue.erase(it);
			}
			else {
				it++;
			}
		}
	}
	//
	//  取消一条消息
	//
	bool MessageQueue::cancel(uint32_t msgId) {
		CMutex::Autolock l(mLock);
		List<MessageEntry>::iterator it = mMsgQueue.begin();
		for (; it != mMsgQueue.end(); it++) {
			if (it->mMessage->uniqueId() == msgId) {
				mMsgQueue.erase(it);
				return true;
			}
		}		
		return false;
	}

	//  type : 速度增加，不变，减少 1，0，-1
	//  speed :当前的速度
	//
	void MessageQueue::onSpeedChange(int32_t type, int32_t speed) {
		if (type == 1) {
			//
			//  速度变快了需要激活计算新的延迟等待时长
			//
			wakeup();
		}
	}
}
