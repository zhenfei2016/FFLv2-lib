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
*  ��־���͹����� 
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
		//  ��ʼ��logsender
		//  type��Ŀ����־������
		//  url : Ŀ����־��·��
		//  ��Ҫ��startupǰ�������õ�
		//
		void initialize(LogSenderType type, const char* url);
		//
		//  ������ֹͣ��־
		//
		bool startup();
		void shutdown();
		//
		//  д��־
		//
		void write(int level,uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list args);
	private:
		LogPipeline* mLogInstance;
	};
}

//
//  �ӹ�logϵͳ
//
extern "C" void FFL_hookLogSystem(FFL::LogSender& sender);
extern "C" void FFL_unhookLogSystem();

#endif