/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineSourceConnector.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/11
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  源连接器，用户其实node的处理，现在支持
*  timer类型 ，PipelineTimerSourceConnector  通过定时器触发的
*  idle类型 ， PipelineIdleSourceConnector    looper空闲的时候触发的
*/


#include <pipeline/FFL_PipelineSourceConnector.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineHandler.hpp>
#include "PipelineLooper.hpp"


namespace FFL {
	PipelineSourceConnector::PipelineSourceConnector() :
		PipelineAsyncConnector(){
	}
	PipelineSourceConnector::PipelineSourceConnector(sp<PipelineLooper>  looper):
		PipelineAsyncConnector(looper) {
		mGenerateTrackId = false;
	}
	PipelineSourceConnector::~PipelineSourceConnector(){
	}
	//
	//  设置是否自动生成track id 
	//
	void PipelineSourceConnector::generateTrackId(bool gen) {
		mGenerateTrackId = gen;
	}
	//
	//  是否是连接状态， 有输入输出端
	//
	bool PipelineSourceConnector::isConnected() {
		return true;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//  空闲的时候也会触发handler的
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class PipelineIdleSourceConnector::IdleHandler : public FFL::IdleHandler
	{
	public:
		IdleHandler(PipelineIdleSourceConnector* input) :mInput(input)
		{
			mIdleMsg = new PipelineMessage();
		}
		void onIdle()
		{			
			if (mInput->mGenerateTrackId) {
				mIdleMsg->trackIdReset(FFL_generateId());
				mIdleMsg->trackStat("type=source");
			}
		    mInput->dispathMessage(mIdleMsg);
		}
	private:
		sp<PipelineMessage> mIdleMsg;
		PipelineIdleSourceConnector* mInput;
	};

	PipelineIdleSourceConnector::PipelineIdleSourceConnector() {
		mIdleHandler = new IdleHandler(this);
	}

	PipelineIdleSourceConnector::PipelineIdleSourceConnector( sp<PipelineLooper>  looper) :
		PipelineSourceConnector(looper){
		mIdleHandler = new IdleHandler(this);
		
	}
	PipelineIdleSourceConnector::~PipelineIdleSourceConnector() {
	}

	//
	//  启动，停止这个连接器
	//
	status_t PipelineIdleSourceConnector::startup() {
		getLooper()->registerIdleHandler(mIdleHandler);		
		return PipelineSourceConnector::startup();
	}	
	//
	//  请求shutdown
	//
	status_t PipelineIdleSourceConnector::requestShutdown() {
		getLooper()->unregisterIdleHandler();
		return PipelineSourceConnector::requestShutdown();
	}
	
	//
	// 开始分派消息
	//
	void PipelineIdleSourceConnector::dispathMessage(const sp<PipelineMessage>& msg) {
		sp<PipelineInput> input = getInput();
		if (!input.isEmpty()) {
			input->dispathMessage(msg);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//  定时器类型的，定时触发handler
	///////////////////////////////////////////////////////////////////////////////////////
	PipelineTimerSourceConnector::PipelineTimerSourceConnector(uint32_t timeMs) :mTimerMs(timeMs) {
		mTimerMessage = new PipelineMessage();		
	}

	PipelineTimerSourceConnector::PipelineTimerSourceConnector(sp<PipelineLooper> looper, uint32_t timeMs):
		PipelineSourceConnector(looper) ,
		mTimerMs(timeMs) {		
		mTimerMessage = new PipelineMessage();
	}
	PipelineTimerSourceConnector::~PipelineTimerSourceConnector() {
	}
	//
	//  启动，停止这个连接器
	//
	status_t PipelineTimerSourceConnector::startup() {
		status_t status=PipelineSourceConnector::startup();	
		if (status == FFL_OK) {			
			activateTimer();
		}

		return status;
	}
	
	//  更新一下下一次执行时间
	//
	void PipelineTimerSourceConnector::updateTimer(uint32_t timeMs) {
		mTimerMs = timeMs;

	}	
	//
	// 开始分派消息
	//
	void PipelineTimerSourceConnector::dispathMessage(const sp<PipelineMessage>& msg) {		
		sp<PipelineInput> input = getInput();
		if (!input.isEmpty()) {
			input->dispathMessage(msg);
		}
		activateTimer();
	}

	//
	//  继续timer的执行
	//
	void PipelineTimerSourceConnector::activateTimer() {
		if (mGenerateTrackId) {
			mTimerMessage->trackIdReset(FFL_generateId());
		}
		tranport(mTimerMessage,mTimerMs * 1000);
	}
}
