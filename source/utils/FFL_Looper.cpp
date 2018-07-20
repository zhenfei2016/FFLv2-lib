 /*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_Looper.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2017/11/25
 *  https://github.com/zhenfei2016/FFL-v2.git
 *  参考 Android Open Source Project 进行修改的
 *  消息循环类
 *
 */

#include <utils/FFL_Utils.hpp>
#include <utils/FFL_Looper.hpp>
#include <utils/FFL_Message.hpp>
#include <utils/FFL_Handler.hpp>
#include <utils/FFL_HandlerManager.hpp>
#include <utils/FFL_MessageQueue.hpp>
#include <list>

namespace FFL {

	class Looper::LooperThread : public Thread 
	{
	public:
		LooperThread(Looper *looper)
			: Thread(),
			  mLooper(looper),
			  mThreadId(0)
		{
		}

		virtual status_t readyToRun(){
			mThreadId = getTid();
			return Thread::readyToRun();
		}

		virtual bool threadLoop(){
			mLooper->setIsLooping(true);
			return mLooper->loop();
		}

		void threadLoopExit(const Thread* t)
		{
			String name = mLooper->mName;
			mLooper->setIsLooping(false);
			mLooper->onQuitLooper(0);
			FFL_LOG_DEBUG("looper(%s) exit", name.c_str());
		}

		bool isCurrentThread() const 
		{
			return mThreadId == FFL_CurrentThreadID();
		}

	protected:
		virtual ~LooperThread() {}

	private:
		Looper *mLooper;
		FFL_ThreadID mThreadId;

		DISABLE_COPY_CONSTRUCTORS(LooperThread);
	};

	Looper::Looper()
		:  mHaveIdleHandler(false)
	{
		mLooping = 0;

		mMsgQueue = new MessageQueue(NULL);
		mHandlerManager = new HandlerManager(this);

		mMsgQuitLooper = new Message();
		mMsgFlush = new Message();
	}

	Looper::Looper(sp<Clock> clock) 
		:  mHaveIdleHandler(false){
		mLooping = 0;
		mMsgQueue = new MessageQueue(clock);
		mHandlerManager = new HandlerManager(this);

		mMsgQuitLooper = new Message();
		mMsgFlush = new Message();		
	}

	Looper::~Looper() {
		stop();

		mHandlerManager->removeAllHandler();
		FFL_SafeFree(mMsgQueue);

		delete mHandlerManager;
		mHandlerManager = NULL;
	}

	void Looper::setName(const char *name)
	{
		mName = name;
	}

	Looper::handler_id Looper::registerHandler(const sp<Handler> &handler) 
	{
		return mHandlerManager->addHandler(handler);
	}

	void Looper::unregisterHandler(Looper::handler_id handlerID)
	{
		mHandlerManager->removeHandler(handlerID);
	}
	//
	//  注册一下空闲处理handler
	//
	void Looper::registerIdleHandler(sp<IdleHandler> handler) {
		CMutex::Autolock autoLock(mLock);
		mIdleHandler = handler;
		if (!handler.isEmpty()) {
			mHaveIdleHandler = true;
		}
		mMsgQueue->wakeup();
	}
	void Looper::unregisterIdleHandler() {
		CMutex::Autolock autoLock(mLock);
		mHaveIdleHandler = false;
		mIdleHandler = NULL;
	}
	status_t Looper::start(int32_t priority) 
	{
		CMutex::Autolock autoLock(mLock);
		if (mThread != NULL ) {
			return FFL_INVALID_OPERATION;
		}


		mThread = new LooperThread(this);
		status_t err = mThread->run(mName.empty() ? "Looper" : mName.c_str(), priority);
		if (err != FFL_NO_ERROR)
		{
			mThread.clear();
		}

		return err;
	}

	status_t Looper::stop() {
		status_t status= FFL_OK;
		status=requestStop();
		if (status != FFL_OK) {
			return status;
		}
		waitStop();
		return FFL_OK;
	}
	//
	//  请求退出，等待退出
	//
	status_t Looper::requestStop() {
		sp<LooperThread> thread;
		{
			CMutex::Autolock autoLock(mLock);
			thread = mThread;
		}

		if (thread == NULL) {
			return FFL_INVALID_OPERATION;
		}

		//
		//  请求线程退出
		//
		if (thread != NULL) {
			thread->requestExit();
		}

		//
		//  
		//
		mMsgQueue->wakeup();
		return FFL_OK;
	}

	status_t Looper::waitStop() {
		sp<LooperThread> thread;
		{
			CMutex::Autolock autoLock(mLock);

			thread = mThread;		
			mThread.clear();			
		}

		//
		//  等待退出
		//
		if (!thread.isEmpty() &&			
			!thread->isCurrentThread()) {
			thread->requestExitAndWait();
		}

		return FFL_OK;
	}
	//
	//  等待到looping，返回是否成功了
	//  waitMs:等待的时长，如果<0 则一直等待
	//
	bool Looper::waitLooping(int32_t waitMs) {	
		CMutex::Autolock l(mLoopingMutex);
		if (waitMs == 0) {
			return mLooping!=0;
		}
		else if (waitMs < 0) {
            while(mLooping==0){
			   mLoopingCond.waitRelative(mLoopingMutex, 100);
            }
		}
		else
		{
			if (mLoopingCond.waitRelative(mLoopingMutex, waitMs)) {
			}
		} 
		return mLooping!=0;
	}
	//
	//  获取当前使用的messagelist
	//
	void Looper::getMessageList(List< sp<Message> >& list)
	{
		mMsgQueue->peekAll(&list);
	}
    
	//
	// 清空等待处理的消息队列
	//
	void Looper::clearMessageList(List< sp<Message> >* list)
	{
		mMsgQueue->getAll(list);
	}
	//
	// 清空等待处理的特定handler消息队列
	//
	void Looper::clearMessageList(List< sp<Message> >* list, Looper::handler_id handlerId) {
		mMsgQueue->getAll(list,handlerId);
	}
	//
	// 取消一条消息
	//
	bool Looper::cancelMessage(uint32_t msgId){
		return mMsgQueue->cancel(msgId);
	}
	//
	// 获取这个looper的messagequeue
	//
	MessageQueue* Looper::getMessageQueue() {
		return mMsgQueue;
	}
	void Looper::setDebug(bool debug)
	{ mMsgQueue->mDebug = debug; }
	//
	// 发送一条消息
	//
	bool Looper::post(const sp<Message> &msg, int64_t delayUs) {
		return mMsgQueue->post(msg, delayUs) >= 0;
	}
	//
	// 发送退出looper消息
	//
	//bool Looper::postQuitMessage(int32_t exitCode)
	//{
	//	sp<Message> msg = new Message();
	//	msg->setWhat(MSGT_SYS_QUIT_LOOPER);
	//	return this->post(msg,0);
	//}

	//
	// 发送刷新消息队列消息,
	// 清空消息队列，不管是否已经到达时间了
	//
	bool Looper::postFlushMessage() {
		FFL_LOG_WARNING("Looper::postFlushMessage no implement.");
		return false;
	}

	bool Looper::loop() 
	{		
		bool haveIdleFlag=false;
		do
		{
			CMutex::Autolock autoLock(mLock);
			if (mThread == NULL) {
				//
				// 退出
				//
				return false;
			}

			haveIdleFlag = mHaveIdleHandler;
		} while (0);


		sp<Message> msg =NULL;
		do {
			if (haveIdleFlag) {
				msg = mMsgQueue->next();
				if (!msg.isEmpty())  break;

				sp<IdleHandler> idleHandler;
				//
				// 空闲处理
				//	
				{
					CMutex::Autolock l(mLock);
					if (mHaveIdleHandler) {
						idleHandler = mIdleHandler.promote();
						mHaveIdleHandler = !idleHandler.isEmpty();
					}
				}

				if (!idleHandler.isEmpty()) {
					idleHandler->onIdle();
				}
				return true;				
			}
			else {
				msg = mMsgQueue->nextBlock();
                
			}
		} while (false);
		
		//
		//  处理消息
		//
        if (msg.isEmpty()) {
            return true;
        }
		if (msg.get() == mMsgQuitLooper.get()) {
			onQuitLooper(0);			
			return false;
		}else if (msg.get() == mMsgFlush.get()) {
		}
		else {		
			mHandlerManager->deliverMessage(msg);
		}		
		return true;
	}	
	//
	//  退出looper
	//
	void Looper::onQuitLooper(const sp<Message> &msg)
	{
		List< sp<FFL::Handler> > list;
		mHandlerManager->getAllHandler(list);

		for (List< sp<FFL::Handler> >::iterator it = list.begin();
			it != list.end(); it++) {
			(*it)->onMessageReceived(NULL);
		}
	}

	bool Looper::isLooping() const {
		return mLooping!=0;
	}

	void Looper::setIsLooping(bool looping) {
		CMutex::Autolock l(mLoopingMutex);
		mLooping = looping?1:0;
		mLoopingCond.signal();
		return;
	}
} 
