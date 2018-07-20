/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Thread.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  基予Android Open Source Project，修改的线程封装
*
*/

#include <thread/FFL_Thread.hpp>

namespace FFL {
	Thread::Thread():		
        mThread(0),
        mStatus(FFL_NO_ERROR),

		mExitPending(false), mRunning(false)		
	{
		mTid=-1;
	}

	Thread::~Thread()
	{
	}

	status_t Thread::readyToRun()
	{
		return FFL_NO_ERROR;
	}

	status_t Thread::run(const char* name, int32_t priority, size_t stack)
	{
		CMutex::Autolock _l(mLock);
		if (mRunning) 
		{			
			return FFL_INVALID_OPERATION;
		}

		// reset status and exitPending to their default value, so we can
		// try again after an error happened (either below, or in readyToRun())
		mStatus = FFL_NO_ERROR;
		mExitPending = false;	

		// hold a strong reference on ourself
		mHoldSelf = this;
		mRunning = true;

		mThread = FFL_CreateThread(_threadLoop, name, this);		
		if (mThread == NULL) {
			mStatus = FFL_ERROR_FAILED;   // something happened!
			mRunning = false;			
			mHoldSelf.clear();  // "this" may have gone away after this.

			return FFL_ERROR_FAILED;
		}
				
		return FFL_NO_ERROR;
	}

	int Thread::_threadLoop(void* user)
	{
		Thread* const self = static_cast<Thread*>(user);

		sp<Thread> strong(self->mHoldSelf);
		wp<Thread> weak(strong);
		self->mHoldSelf.clear();

		FFL_ThreadID tid= self->mTid = FFL_CurrentThreadID();
		FFL_LOG_DEBUG("Thread(%d) run", tid);
		bool first = true;
		bool exec_thread_exit = false;
		self->threadLoopStart();
		do {
			bool result;
			if (first) {
				first = false;
				self->mStatus = self->readyToRun();
				result = (self->mStatus == FFL_NO_ERROR);

				if (result && !self->exitPending()) {
					// Binder threads (and maybe others) rely on threadLoop
					// running at least once after a successful ::readyToRun()
					// (unless, of course, the thread has already been asked to exit
					// at that point).
					// This is because threads are essentially used like this:
					//   (new ThreadSubclass())->run();
					// The caller therefore does not retain a strong reference to
					// the thread and the thread would simply disappear after the
					// successful ::readyToRun() call instead of entering the
					// threadLoop at least once.
					result = self->threadLoop();
				}
			}
			else {
				result = self->threadLoop();
			}

			// establish a scope for mLock
			{
				CMutex::Autolock _l(self->mLock);
				if (result == false || self->mExitPending) {
					self->mExitPending = true;
					self->mRunning = false;
					
					// clear thread ID so that requestExitAndWait() does not exit if
					// called by a new thread using the same thread ID as this one.
					self->mThread = 0;
					
					self->threadLoopExit(self);
					exec_thread_exit = true;

					// note that interested observers blocked in requestExitAndWait are
					// awoken by broadcast, but blocked on mLock until break exits scope
					self->mThreadExitedCondition.broadcast();
					break;
				}
			}

			// Release our strong reference, to let a chance to the thread
			// to die a peaceful death.
			strong.clear();
			// And immediately, re-acquire a strong reference for the next loop
			strong = weak.promote();
		} while (!strong.is_empty());

		if(!exec_thread_exit)
		   self->threadLoopExit(0);
		
		FFL_LOG_DEBUG("Thread(%d) quit", tid);

		return 0;
	}

	void Thread::requestExit()
	{
		CMutex::Autolock _l(mLock);
		mExitPending = true;
	}

	status_t Thread::requestExitAndWait()
	{
		CMutex::Autolock _l(mLock);
		if (mTid == FFL_CurrentThreadID()) {
			FFL_LOG_WARNING(
				"Thread (this=%p): don't call waitForExit() from this "
				"Thread object's thread. It's a guaranteed deadlock!",
				this);

			return FFL_WOULD_BLOCK;
		}

		mExitPending = true;

		while (mRunning == true) {
			mThreadExitedCondition.wait(mLock);
		}
		// This next line is probably not needed any more, but is being left for
		// historical reference. Note that each interested party will clear flag.
		mExitPending = false;

		return mStatus;
	}

	status_t Thread::join()
	{
		CMutex::Autolock _l(mLock);
		if (mTid == FFL_CurrentThreadID()) {
			FFL_LOG_WARNING(
				"Thread (this=%p): don't call join() from this "
				"Thread object's thread. It's a guaranteed deadlock!",
				this);

			return FFL_WOULD_BLOCK;
		}

		while (mRunning == true) {
			mThreadExitedCondition.wait(mLock);
		}

		return mStatus;
	}

	bool Thread::isRunning() const {
		CMutex::Autolock _l(mLock);
		return mRunning;
	}


	FFL_ThreadID Thread::getTid() const
	{	
		CMutex::Autolock _l(mLock);
		FFL_ThreadID tid;
		if (mRunning) {			
			tid = FFL_GetThreadID(mThread);
		}
		else {
			FFL_LOG_WARNING("Thread (this=%p): getTid() is undefined before run()", this);
			tid = -1;
		}
		return tid;
	}


	bool Thread::exitPending() const
	{
		CMutex::Autolock _l(mLock);
		return mExitPending;
	}

	void Thread::threadLoopStart()
	{
	}
	void Thread::threadLoopExit(const Thread* t)
	{
	}
}
