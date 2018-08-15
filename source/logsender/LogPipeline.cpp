/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogPipeline.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*  ��־������ˮ�ߣ�����������������
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
	//  type��Ŀ����־������
	//  url : Ŀ����־��·��	
	//  ����Ŀ���ļ���
	//
	void LogPipeline::setTargetUrl(LogSenderType type, const char* url) {
		mType = type;
		mUrl = url;
		if (isStartup()) {
			if (!mUploader.isEmpty()) {
				mUploader->refrushWriter(mType,mUrl);
			}
		}
	}
	LogPipeline::~LogPipeline() {
		shutdown();
		FFL_SafeFree(mManager);
	}

	void LogPipeline::getTargetUrl(LogSenderType& type, String& url) {
		type = mType;
		url = mUrl;
	}

	bool LogPipeline::startup() {		
		if (isStartup()) {
			return true;
		}

		mPipeline = new FFL::Pipeline("");
		mManager = new FFL::NodeManager(mPipeline);

		mCollector->reset();
		mCollector->create(mManager);
		
		LogWriterCreator* writerCreator = new LogWriterCreator();
		writerCreator->create(mManager);
		
		LogUploader* uploader = new LogUploader();
		uploader->refrushWriter(mType, mUrl);
		uploader->create(mManager);		

			
		//
		//  reader   ������־���ݸ�uploader
		//  uploader �����������ݸ� writerCreator �ô���writer
		//  writerCreator ����writer�ɹ��󣬷�����uploader��writer
		//
		//  writer:�����ܵ����ݴ洢��ʽ���������磬���صȵ�
		//  ����Ҫ��writer�Ĵ�����ʽ
		//
		mCollector->connectOutputToUploader(uploader, "uploader",0);
		uploader->connectOutputToCreator(writerCreator, "creator-writer", 0);
		writerCreator->connectOutputWriterTarger(uploader, "update-writer", 0);
		mUploader = uploader;
		mPipeline->startup();
		return true;
	}

	void LogPipeline::shutdown() {		
		if (isStartup()) {			
			mPipeline->shutdownAndWait();
		}		
		mUploader = NULL;
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
		FFL::formatString(info, "%s %s pid:%d tid:%d ",
			FFL_LogGetLevelString(level),timeFormat, FFL_getProcessId(),(int)FFL_CurrentThreadID());
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


