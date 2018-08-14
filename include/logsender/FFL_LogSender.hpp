/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_LogSender.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  日志发送管理器 
*/
#ifndef _FFL_LOG_SENDER_HPP_
#define _FFL_LOG_SENDER_HPP_

#include <FFL.h>
#include <logsender/FFL_LogSenderType.hpp>

namespace FFL {	
	class LogPipeline;
	class LogSender {
	public:
		LogSender();
		~LogSender();
		//
		//  初始化logsender
		//  type：目标日志的类型
		//  url : 目标日志的路径
		//  需要在startup前进行设置的
		//
		void initialize(LogSenderType type, const char* url);
		//
		//  启动，停止日志
		//
		bool startup();
		void shutdown();
		//
		//  写日志
		//
		void write(int level,uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list args);
	private:
		LogPipeline* mLogInstance;
	};
}

//
//  接管log系统
//
extern "C" void FFL_hookLogSystem(FFL::LogSender& sender);
extern "C" void FFL_unhookLogSystem();

#endif