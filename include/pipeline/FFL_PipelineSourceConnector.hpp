/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineSourceConnector.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/11
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  源连接器，用户其实node的处理，现在支持 
*  timer类型 ，PipelineTimerSourceConnector  通过定时器触发的
*  idle类型 ， PipelineIdleSourceConnector    looper空闲的时候触发的
*/

#ifndef _FFL_PIPELINE_SOURCE_CONNECTOR_HPP_
#define _FFL_PIPELINE_SOURCE_CONNECTOR_HPP_


#include <pipeline/FFL_PipelineAsyncConnector.hpp>
#include <thread/FFL_Mutex.hpp>


namespace FFL
{
	class Handler;
	class PipelineInput;
	class PipelineOutput;
	class PipelineMessage;
	
	class PipelineLooper;

	//
	//  驱动源节点的连接器， 这个连接器会源源不断的向Input接口push数据
	//
	class PipelineSourceConnector : public PipelineAsyncConnector {
	public:
		PipelineSourceConnector();
		PipelineSourceConnector(sp<PipelineLooper>  looper);
		virtual ~PipelineSourceConnector();
		//
		//  设置是否自动生成track id 
		//
		void generateTrackId(bool gen);
		//
		//  是否是连接状态， 有输入输出端
		//
		virtual bool isConnected();

	
		bool  mGenerateTrackId;
	};


	//
	//空闲时候就会push数据过去  
    //
	class PipelineIdleSourceConnector : public PipelineSourceConnector {
	public:
		PipelineIdleSourceConnector();
		PipelineIdleSourceConnector(sp<PipelineLooper> looper);
		~PipelineIdleSourceConnector();

		//
		//  启动，停止这个连接器
		//
		status_t startup();		
		//
		//  请求shutdown,等待shutdown结束
		//
		status_t requestShutdown();		
		//
		// 开始分派消息
		//
		void dispathMessage(const sp<PipelineMessage>& msg);
	protected:
		//
		//  消息处理句柄
		//
		class IdleHandler;
		sp<IdleHandler> mIdleHandler;
	};


	//
	//定时器类型的连接器，定时触发执行
	//
	class PipelineTimerSourceConnector : public PipelineSourceConnector {
	public:
		PipelineTimerSourceConnector(uint32_t timeMs);		
		PipelineTimerSourceConnector(sp<PipelineLooper> looper, uint32_t timeMs);
		virtual  ~PipelineTimerSourceConnector();

		//
		//  启动，停止这个连接器
		//
		status_t startup();	

		//
		//  更新一下下一次执行时间
		//
		void updateTimer(uint32_t timeMs);
		
		//
		// 开始分派消息
		//
		virtual void dispathMessage(const sp<PipelineMessage>& msg);
	private:
		//
		//  继续timer的执行
		//
		void activateTimer();
	protected:	
		uint32_t mTimerMs;
		sp<PipelineMessage> mTimerMessage;
	};
}


#endif
