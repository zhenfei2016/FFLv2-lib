/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Coroutine.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/04
*  https://github.com/zhenfei2016/FFL-v2.git
*
*
*/

#include <thread/FFL_Coroutine.hpp>
#include <thread/FFL_Thread.hpp>

namespace FFL {
	class Coroutine : public  Thread
	{
		friend class ThreadCoroutine;
	public:
		Coroutine(ICoroutineHandler* h):mHandler(h)
		{
		}
		
		// Derived class must implement threadLoop(). The thread starts its life
		// here. There are two ways of using the Thread object:
		// 1) loop: if threadLoop() returns true, it will be called again if
		//          requestExit() wasn't called.
		// 2) once: if threadLoop() returns false, the thread will exit upon return.
		virtual bool threadLoop()
		{
			do{

			}while(mHandler->loop()==FFL_NO_ERROR);
	
			return false;
		}
	private:
		ICoroutineHandler* mHandler;
	};

	ThreadCoroutine::ThreadCoroutine(ICoroutineHandler* h):mHandler(h)
	{
		mThread=new Coroutine(mHandler);
	}
	ThreadCoroutine::~ThreadCoroutine()
	{

	}

	status_t ThreadCoroutine::start()
	{	
		return mThread->run();
	}
	void ThreadCoroutine::stop()
	{		
	    mThread->requestExitAndWait();
	}

	void ThreadCoroutine::interrupt()
	{		
		mThread->requestExit();
	}

	status_t ThreadCoroutine::pull()
	{
		return mThread->exitPending()?FFL_ERROR_FAILED:FFL_NO_ERROR;

	}

	int ThreadCoroutine::cid()
	{
		return 0;
	}

};



