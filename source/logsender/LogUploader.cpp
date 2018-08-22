/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogUploader.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*  日志上传
*
*/
#include "LogUploader.hpp"
#include "LogMessages.hpp"
#include "LogMessageId.hpp"

namespace FFL {
	LogUploader::LogUploader():mType(LOG_ST_NONE) {
		setName("LogUploader");
		mCreateWriterTime = 0;
		mWriter = NULL;
	}

	LogUploader:: ~LogUploader() {
	}
	//
	//  刷新一下新的writer，会请求重新创建
	//
	void LogUploader::refrushWriter(LogSenderType type,const String& url) {
		//
		//  创建目标
		//
		mType=type;
		mUrl=url;	
		
		if (mOutputWriter.isValid()) {
			createWriter();
		}
	}
	//
	//  输出到next中
	//
	void LogUploader::connectOutputToCreator(NodeBase* next, const char* name, void* userdata) {
		mOutputWriter = connectNext(next, name, userdata);
	}
	FFL::sp<FFL::PipelineConnector > LogUploader::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input,
		void* userdata) {
		FFL::PipelineAsyncConnector* conn;
		if (mLooper == NULL) {
			conn= new FFL::PipelineAsyncConnector();
			mLooper=conn->getLooper();
		}
		else {
			conn= new FFL::PipelineAsyncConnector(mLooper);
		}
		return conn;
	}
	bool LogUploader::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		switch (msg->getType())
		{
		case LOG_MESSAGE_UPDATE_WRITER:
			handleUpdateWriter(msg, userdata);
			msg->consume(this);
			break;
		case LOG_MESSAGE_READER:
			handleLogInfo(msg,userdata);
			msg->consume(this);
			break;
		default:
			msg->consume(this);
			break;
		}		
		return true;
	}
	void LogUploader::handleLogInfo(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {		
		if (!mWriter.isEmpty()) {
			IOWriter* writer=mWriter->getWriter();			
			if (writer) {
				size_t writed = 0;
				LogInfo* info = (LogInfo*)(msg->getPayload());
				if (FFL_OK == writer->write((void*)(info->mInfo.c_str()),
					info->mInfo.size(), &writed)) {
					return;
				}
			}
		}
		tryCreateWriter();
	}

	//
	//  更新writer
	//
	void LogUploader::handleUpdateWriter(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		LogWriterMessage* payload=msg->getPayloadT<LogWriterMessage>();
		mWriter = payload->mWriter;
	}


	void LogUploader::tryCreateWriter() {
		if (mCreateWriterTime == 0 ||
			FFL_getNowUs() - mCreateWriterTime > (1000 * 1000) * 60) {
			//
			//  是否跟上一次大于一分钟了
			//
			createWriter();
		}
	}
	void LogUploader::createWriter() {
		mCreateWriterTime = FFL_getNowUs();
		FFL::PipelineMessage* controlMsg = new FFL::PipelineMessage(LOG_MESSAGE_CREATE_WRITER);
		LogWriterUrlMessage* urlMessage= new LogWriterUrlMessage();
		urlMessage->mType = mType;
		urlMessage->mUrl = mUrl;
		controlMsg->setPayload(urlMessage);

		if (FFL_OK != postMessage(mOutputWriter.getId(), controlMsg)) {
			controlMsg->consume(this);
		}
	}

}


