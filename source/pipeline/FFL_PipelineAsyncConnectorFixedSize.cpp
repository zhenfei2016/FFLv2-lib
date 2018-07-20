/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineAsyncConnectorFixedsize.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/10
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  node异步连接器,进行消息处理中消息的缓存异步处理
*  具有固定大小的connecter ，待处理的消息不会大于这个大小
*/


#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <utils/FFL_Handler.hpp>
#include "PipelineLooper.hpp"

namespace FFL {
	class DefaultMessageSizeCalculator : public PipelineAsyncConnectorFixSize::MessageSizeCalculator {
	public:
		uint32_t calcSize(std::list< sp<PipelineMessage> >& msgList) {
			return msgList.size();
		}		
	};
	static DefaultMessageSizeCalculator gDefaultMessageSizeCalculator;

	PipelineAsyncConnectorFixSize::PipelineAsyncConnectorFixSize(uint32_t capacity) :
		PipelineAsyncConnector(),
		mSize(0) ,
		mIsFull(false),
		mMessageSizeCalculator(NULL)
	{
		mHasShutdownRequest = false;
		setFullFlag(IF_FULL_WAIT);
		setCapacity(capacity);
	}
	PipelineAsyncConnectorFixSize::PipelineAsyncConnectorFixSize(uint32_t capacity, sp<PipelineLooper>  looper):
		PipelineAsyncConnector(looper),
		mSize(0) ,
		mIsFull(false),
		mMessageSizeCalculator(NULL)
	{
		setFullFlag(IF_FULL_WAIT);
		setCapacity(capacity);		
	}
	PipelineAsyncConnectorFixSize::~PipelineAsyncConnectorFixSize() 
	{
		if (mMessageListCache.size() > 0) {
			FFL_LOG_WARNING("~PipelineAsyncConnectorFixSize  message cache not empty size=%d",mMessageListCache.size());
		}
		if (mMessageSizeCalculator)
		{
			delete mMessageSizeCalculator;
			mMessageSizeCalculator = NULL;
		}
	}
	//
	//  请求shutdown,等待shutdown结束
	//
	status_t PipelineAsyncConnectorFixSize::requestShutdown() {
		PipelineAsyncConnector::requestShutdown();
		{//如果在等待中需要通知一下
			CMutex::Autolock l(&mMutex);
			//if(mIsFull)
			mHasShutdownRequest = true;
			mCond.signal();
		}

		return FFL_OK;

	}
	status_t PipelineAsyncConnectorFixSize::waitShutdown() {
		PipelineAsyncConnector::waitShutdown();
		//
		//未处理的消息，需要通知这个消息取消了
		// 这时候不需要锁了，线程已经结束了
		//
		MessageList::iterator it = mMessageListCache.begin();
		for (; it != mMessageListCache.end(); it++) {
			(*it)->consume(this);
		}
		mMessageListCache.clear();
		return FFL_OK;
	}
	//
	// 获取，设置容量
	//
	void PipelineAsyncConnectorFixSize::setCapacity(uint32_t capacity) {
		CMutex::Autolock l(&mMutex);
		mCapacity = FFL_MAX(capacity,1);
	}
	uint32_t PipelineAsyncConnectorFixSize::getCapacity() const {
		CMutex::Autolock l(&mMutex);
		return mCapacity;
	}
	//
	// 连接器转发消息
	//
	status_t PipelineAsyncConnectorFixSize::tranport(const sp<PipelineMessage> &msg, int64_t delayUs){
		int64_t startUs = FFL_getNowUs();
		{			
			CMutex::Autolock l(&mMutex);						
			while ( mSize >= mCapacity && !isShutdowned() && !mHasShutdownRequest) {
				FFL_LOG_INFO("PipelineAsyncConnectorFixSize(%s) is full. size=%d capacity=%d", getName(), mSize,mCapacity);
				if (mFullFlags.hasFlags(IF_FULL_REMOVE_FIRST)) {
					//
					//  移除待处理的第一条
					//
					sp<PipelineMessage> firstMsg=mMessageListCache.back();
					mMessageListCache.pop_back();
					if(!getLooper()->clearMessage(firstMsg)){
						FFL_LOG_WARNING("remove first . not find msg");
					}else {
						firstMsg->consume(this);
					}
					break;
				}
				else if (mFullFlags.hasFlags(IF_FULL_REMOVE_LAST)) {
					//
					//  移除待处理的最后一条
					//
					sp<PipelineMessage> lastMsg = mMessageListCache.front();
					mMessageListCache.pop_front();
					if (!getLooper()->clearMessage(lastMsg) ) {
						FFL_LOG_WARNING("remove last . not find msg");
					}
					else {
						lastMsg->consume(this);
					}
					break;
				}else {
					//
					//  一直等到有位置时
					//
					mCond.wait(mMutex);
				}
			}

			if (isShutdowned()|| mHasShutdownRequest) {
				FFL_LOG_INFO("PipelineAsyncConnectorFixSize(%s) is shutdown.",getName());
				return FFL_ERROR_FAIL;
			}

			//
			//  更新大小
			//
			mMessageListCache.push_front(msg);
			if (mMessageSizeCalculator != NULL) {
				mSize = mMessageSizeCalculator->calcSize(mMessageListCache);
			}
			else {
				mSize = gDefaultMessageSizeCalculator.calcSize(mMessageListCache);
			}
			mIsFull = (mSize >= mCapacity);
			FFL_LOG_INFO("PipelineAsyncConnectorFixSize(%s) insert size=%d capacity=%d", getName(), mSize,mCapacity);
		}

		//
		//  传递，分发消息
		//

		if (delayUs != 0 ) {
			int64_t nowUs = FFL_getNowUs();
			delayUs = delayUs - (nowUs - startUs);
			delayUs = FFL_MAX(0, delayUs);
		}
        status_t status = PipelineAsyncConnector::tranport(msg, delayUs);
		if (FFL_OK != status) {
			consumer(msg);
		}
		return status;
	}
	//
	//  清空转发的消息
	//
	void PipelineAsyncConnectorFixSize::clearMessage() {
		FFL_LOG_DEBUG("PipelineAsyncConnectorFixSize::clearMessage(%s)",getName() );
		CMutex::Autolock l(&mMutex);				
		mMessageListCache.clear();
		PipelineAsyncConnector::clearMessage();
		//
		//  重新计算一下现在消息的大小
		//
		reCalcSizeLock();
	}

	//
	//  这个消息已经给消费了，
	//
	void PipelineAsyncConnectorFixSize::consumer(const sp<PipelineMessage> &msg) {
		CMutex::Autolock l(&mMutex);

		//
		//  移除这个缓存的消息
		//
		bool findMsg = false;		
		MessageList::iterator it = mMessageListCache.begin();
		for (; it != mMessageListCache.end(); it++) {
			if ((*it).get() == msg.get() ){
				mMessageListCache.erase(it);
				findMsg = true;
				break;
			}
		}
		if (!findMsg) {
			FFL_LOG_INFO("PipelineAsyncConnectorFixSize not find msg");
			return;
		}

		//
		//  重新计算一下现在消息的大小
		//
		reCalcSizeLock();
	}
	//
	// 获取待处理的消息的大小
	//
	int64_t PipelineAsyncConnectorFixSize::getPendingMessageSize() {
		return mSize;
	}
	void PipelineAsyncConnectorFixSize::reCalcSizeLock() {
		//
		//  重新计算一下现在消息的大小
		//
		if (mMessageSizeCalculator != NULL) {
			mSize = mMessageSizeCalculator->calcSize(mMessageListCache);
		}
		else {
			mSize = gDefaultMessageSizeCalculator.calcSize(mMessageListCache);
		}

		if (mIsFull && mSize < mCapacity) {
			FFL_LOG_INFO("PipelineAsyncConnectorFixSize(%s) signal. size=%d capacity=%d",getName(), mSize, mCapacity);
			mIsFull = false;
			mCond.signal();
		}
	}
	//
	// 设置满了情况的处理标志
	//
	void PipelineAsyncConnectorFixSize::setFullFlag(PipelineAsyncConnectorFixSize::FullFlag flag)
	{
		mFullFlags.resetFlags(flag);
	}

	//		
	//  设置消息列表的总大小计算器
	//  这个函数需要在connect启动前进行设置,启动后设置可能会出现未知异常
	//  calc: 类释放是，会进行delete calc
	//
	void PipelineAsyncConnectorFixSize::setMessageSizeCalculator(MessageSizeCalculator* calc) {
		mMessageSizeCalculator = calc;
	}
}
