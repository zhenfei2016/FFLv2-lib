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
*  日志发送管理器
*/

#include <logsender/FFL_LogSender.hpp>
#include <net/base/FFL_Net.h>
#include "LogPipeline.hpp"

namespace FFL {
	LogSender::LogSender() {
		FFL_socketInit();
		mLogInstance = new LogPipeline();
	}
	LogSender::~LogSender() {
		mLogInstance->shutdown();
		FFL_SafeFree(mLogInstance);
	}
	//
	//  初始化logsender
	//
	void LogSender::initialize(LogSenderType type, const char* url) {
		mLogInstance->setLogType(type, url);
	}
	bool LogSender::startup() {
		mLogInstance->startup();
		return true;
	}
	void LogSender::shutdown() {
		mLogInstance->shutdown();
	}

	void LogSender::write(int level, uint8_t* data, int32_t len) {
		mLogInstance->write(level, data, len);
	}
	void LogSender::write(int level, const char* tag, const char *format, va_list args) {
		mLogInstance->write(level, tag,format,args);
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
//  接管log系统
//
extern "C"  void FFL_hookLogSystem(FFL::LogSender& sender) {
	FFL_LogHook(printLogToSender, &sender);
}
extern "C"  void FFL_unhookLogSystem() {
	FFL_LogHook(0, 0);
}