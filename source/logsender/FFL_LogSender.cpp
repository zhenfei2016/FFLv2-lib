/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_LogSender.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  ��־���͹�����
*/

#include <logsender/FFL_LogSender.hpp>
#include <net/base/FFL_Net.h>
#include "LogPipeline.hpp"

namespace FFL {
	LogSender::LogSender():mLogInstance(NULL){
		FFL_socketInit();
		mLogStartuped = false;
		mType = LOG_ST_NONE;
	}
	LogSender::~LogSender() {
		shutdown();
		{
			CMutex::Autolock l(mLock);
			FFL_SafeFree(mLogInstance);
		}
	}	
	//		
	//  type��Ŀ����־������
	//  url : Ŀ����־��·��
	//  startupǰ��Ҫ�������õ�
	//  startup�����ã������Ŀ���ļ���
	//
	void LogSender::setTargetUrl(LogSenderType type, const char* url) {
		mType = type;
		mUrl = url;
		if (mLogInstance) {
			mLogInstance->setTargetUrl(type, url);
		}

	}
	bool LogSender::startup() {	
		CMutex::Autolock l(mLock);
		if (!mLogStartuped) {
			FFL_SafeFree(mLogInstance);
		}

		mLogInstance = new LogPipeline();
		mLogInstance->setTargetUrl(mType, mUrl.c_str());
		mLogInstance->startup();
		mLogStartuped = true;
		return true;
	}
	void LogSender::shutdown() {
		CMutex::Autolock l(mLock);
		mLogStartuped = false;
		if (mLogInstance) {
			mLogInstance->shutdown();	
			//
			//  Ϊ��write������  mLogInstance�ӳ�delete
			//

		}
	}

	void LogSender::write(int level, uint8_t* data, int32_t len) {
		if (mLogStartuped&&mLogInstance) {
			mLogInstance->write(level, data, len);
		}
	}
	void LogSender::write(int level, const char* tag, const char *format, va_list args) {
		if (mLogStartuped&&mLogInstance) {
			mLogInstance->write(level, tag, format, args);
		}
	}
}

static int printLogToSender(int level, const char* tag, const char *format, va_list args, void* userdata) {
	FFL::LogSender* sender = (FFL::LogSender*)userdata;
	if (sender) {
		sender->write(level, tag, format, args);
	}
	return 1;
}
//
//  �ӹ�logϵͳ
//
extern "C"  void FFL_hookLogSystem(FFL::LogSender& sender) {
	FFL_LogHook(printLogToSender, &sender);
}
extern "C"  void FFL_unhookLogSystem() {
	FFL_LogHook(0, 0);
}