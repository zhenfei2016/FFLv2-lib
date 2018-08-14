/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogCollector.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/14 
*  https://github.com/zhenfei2016/FFL-v2.git
*  ��־�ռ�
*
*/
#include "LogCollector.hpp"
#include "LogMessageId.hpp"
#include "LogMessages.hpp"

namespace FFL {
	LogCollector::LogCollector() {
		setName("LogCollector");
		mMessageCache = new FFL::PipelineMessageCache(LOG_MESSAGE_READER);
	}

	LogCollector::~LogCollector() {
	}
	//
	// ������־�ռ���
	//
	void LogCollector::reset() {		
		destroy();		
		mOutputUploader.reset();

		{
			FFL::CMutex::Autolock l(mInfoLock);
			mInfoList.clear();
		}
	}
	//
	//  ��reader���������
	//
	void LogCollector::collect(FFL::String& info) {
		FFL::CMutex::Autolock l(mInfoLock);
		mInfoList.push_back(info);
	}
	//
	//  �����next��
	//
	void LogCollector::connectOutputToUploader(NodeBase* next, const char* name, void* userdata) {
		mOutputUploader = connectNext(next, name, userdata);
	}
	//
	//  �ɹ�������node
	//
	void LogCollector::onCreate() {	
		connectTimerInput("LogCollector", 1000, 0);
	}
	//
	//  ��ȡ����ѭ������
	//
	bool LogCollector::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		FFL::String infoString;
		{
			FFL::CMutex::Autolock l(mInfoLock);
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


