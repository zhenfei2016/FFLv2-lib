/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogPipeline.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*  日志处理流水线，串起来整个的流程
*
*/
#include <pipeline/FFL_NodeBase.hpp>
#include <logsender/FFL_LogSenderType.hpp>
#include "LogCollector.hpp"

namespace FFL {
	class LogUploader;
	class LogPipeline  {
	public:
		LogPipeline();
		~LogPipeline();	
		//		
		//  type：目标日志的类型
		//  url : 目标日志的路径	
		//  更新目标文件的
		//
		void setTargetUrl(LogSenderType type, const char* url);
		void getTargetUrl(LogSenderType& type, String& url);
		//
		//  启动，停止LogPipeline
		//
		bool startup();
		void shutdown();        
		//
		//  写日志信息
		//
		void write(int level, uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list v);
	private:
		bool isStartup();
	private:
		FFL::NodeManager* mManager;
		FFL::sp<FFL::Pipeline> mPipeline;
		//
		// 收集器
		//
		FFL::sp<LogCollector> mCollector;
		FFL::sp<LogUploader> mUploader;
		
		//
		//  创建的类型
		//
		LogSenderType mType;
		String mUrl;
	};
}


