/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogPipeline.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*  日志处理流水线，串起来整个的流程
*
*/
#include "LogPipeline.hpp"
#include "LogCollector.hpp"
#include "LogUploader.hpp"
#include "LogWriterCreator.hpp"

namespace FFL {	
	LogPipeline::LogPipeline():mCollector(NULL){
		mCollector = new LogCollector();
		mManager = NULL;
	}
	//
	//
	//
	void LogPipeline::setLogType(LogSenderType type, const char* url) {
		mType = type;
		mUrl = url;
	}
	LogPipeline::~LogPipeline() {
		FFL_SafeFree(mManager);
	}

	bool LogPipeline::startup() {		
		if (isStartup()) {
			return true;
		}

		mPipeline = new FFL::Pipeline("");
		mManager = new FFL::NodeManager(mPipeline);

		mCollector->reset();
		mCollector->create(mManager);
		
		LogWriterCreator* writerCreator = new LogWriterCreator(mType,mUrl.c_str());
		writerCreator->create(mManager);
		
		LogUploader* uploader = new LogUploader();
		uploader->create(mManager);

			
		//
		//  reader   生产日志数据给uploader
		//  uploader 生产控制数据给 writerCreator 让创建writer
		//  writerCreator 创建writer成功后，反馈给uploader的writer
		//
		//  writer:是最总的数据存储方式，可以网络，本地等等
		//  ，主要看writer的创建方式
		//
		mCollector->connectOutputToUploader(uploader, "uploader",0);
		uploader->connectOutputToCreator(writerCreator, "creator-writer", 0);
		writerCreator->connectOutputWriterTarger(uploader, "update-writer", 0);

		mPipeline->startup();
		return true;
	}

	void LogPipeline::shutdown() {		
		if (isStartup()) {
			mPipeline->shutdownAndWait();
		}
		FFL_SafeFree(mManager);
		mPipeline = NULL;
	}

	void LogPipeline::write(int level,uint8_t* data, int32_t len) {
		if (!isStartup()) {
			return;
		}

		char timeFormat[256] = {};
		FFL_getNowString(timeFormat);

		FFL::String info;
		FFL::formatString(info, "%s:%s tid = %d ",
			FFL_LogGetLevelString(level),timeFormat, (int)FFL_CurrentThreadID());
		info.append((const char*)data);		

		if (!mCollector.isEmpty()) {
			mCollector->collect(info);
		}
	}

	void LogPipeline::write(int level, const char* tag, const char *format, va_list v) {
		char str[4096] = {};
		vsnprintf(str, 4096 - 1,format, v);
		write(level, (uint8_t*)str, strlen(str));
	}	

	bool LogPipeline::isStartup() {
		return mPipeline != NULL;
	}
}


