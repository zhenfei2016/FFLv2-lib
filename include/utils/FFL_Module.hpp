/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Module.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/29
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*/
#ifndef _FFL_MODULE_HPP_
#define _FFL_MODULE_HPP_
#include <thread/FFL_Mutex.hpp>
#include <thread/FFL_Thread.hpp>

namespace FFL {
	class ModuleThread;

	class Module {
	public:
		Module();
		virtual ~Module();

		//
		//  启动，
		//  thread： 使用使用这个县城进行eventloop
		//           =NULL , 需要外部调用eventLoop 
		//           !=NULL , 在这个thread中执行eventloop  
		//  返回是否启动成功
		//
		bool start(ModuleThread* thread) ;
		//
		//   如果start使用了EventloopThread，则stop会阻塞到线程退出
		//   否则则仅仅置一下标志
		//
		void stop() ;
	public:
		//
		//  调用。start，stop会触发onStart,onStop的执行
		//  onStart :表示准备开始了 ,可以做一些初始化工作
		//  onStop :表示准备停止了 ,可以做停止前的准备，想置一下信号让eventloop别卡住啊 
		//  在这些函数中，不要再调用自己的函数，例如：start,stop, isStarted等
		//
		virtual bool onStart();
		virtual void onStop();
		//
		//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
		//   waitTime:输出参数，下一次执行eventLoop等待的时长
		//   true  : 继续进行下一次的eventLoop
		//   false : 不需要继续执行eventloop
		//
		virtual bool eventLoop(int32_t* waitTime) = 0;	
		//
		//  是否启动状态
		//
		bool isStarted() const;
	protected:
		volatile bool mStarted;
		mutable CMutex mLock;
		FFL::sp<ModuleThread> mModuleThread;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//   跑module循环的线程
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class ModuleThread : public Thread {
		friend class Module;
	public:
		ModuleThread(const char* name);
		~ModuleThread();
		//
		//   设置关联到哪一个module上
		//
		void setModule(Module* module);
		//
		//  获取线程名称
		//
		const char* getName() const;
	protected:
		/*  Thread  */
		virtual status_t  run();
		virtual void     requestExit();
		virtual status_t requestExitAndWait();	
		/*  Thread  */
		virtual bool threadLoop();
	private:		
		const char* mName;
		Module* mModule;
		int32_t mModuleWaitMs;

		CMutex mMutex;
		CCondition mCond;
	};
}

#endif
