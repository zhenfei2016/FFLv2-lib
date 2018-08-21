/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogCollector.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/14 
*  https://github.com/zhenfei2016/FFL-v2.git
*  日志收集
*
*/
#include "LogCollector.hpp"
#include "LogMessageId.hpp"
#include "LogMessages.hpp"

namespace FFL {
	//
	//  处理间隔中最多缓存gMaxLogNum条日志
	//
	static int32_t gMaxLogNum = 100;

	//
	//  日志收集的处理间隔，gTimerTick_ms处理一次
	//
	static uint32_t gTimerTick_ms = 1000;
	LogCollector::LogCollector():mDiscardNum(0){
		setName("LogCollector");
		mMessageCache = new FFL::PipelineMessageCache(LOG_MESSAGE_READER);
	}

	LogCollector::~LogCollector() {
	}
	//
	// 重置日志收集器
	//
	void LogCollector::reset() {		
		destroy();		
		mOutputUploader.reset();

		{
			FFL::CMutex::Autolock l(mInfoLock);
			mInfoList.clear();
			mDiscardNum = 0;
		}
	}
	//
	//  向reader中填充数据
	//
	void LogCollector::collect(FFL::String& info) {
		FFL::CMutex::Autolock l(mInfoLock);
		if (mInfoList.size() >= gMaxLogNum) {
			mDiscardNum++;
			mInfoList.pop_front();
		}

		mInfoList.push_back(info);
	}
	//
	//  输出到next中
	//
	void LogCollector::connectOutputToUploader(NodeBase* next, const char* name, void* userdata) {
		mOutputUploader = connectNext(next, name, userdata);
	}
	//
	//  成功创建了node
	//
	void LogCollector::onCreate() {	
		connectTimerInput("LogCollector", gTimerTick_ms, 0);
	}
	//
	//  获取数据循环函数
	//
	bool LogCollector::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		FFL::String infoString;
		{
			FFL::CMutex::Autolock l(mInfoLock);
			if (mDiscardNum > 0) {
				FFL::formatString(infoString, "FFLog discard log count=(%d)", mDiscardNum);				
				mDiscardNum = 0;
			}

			while (mInfoList.size() > 0) {
				infoString += mInfoList.front() + "\r";
				mInfoList.pop_front();
			}			
		}

		if (infoString.size()) {
			LogInfo* info = NULL;
			FFL::sp<FFL::PipelineMessage> msg = mMessageCache->createMessage(&info, LOG_MESSAGE_READER);
			info->mInfo = infoString;
			if (FFL_OK != postMessage(mOutputUploader.getId(), msg)) {
				msg->consume(this);
			}
		}
		return true;
	}
}


