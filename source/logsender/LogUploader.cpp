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
    //
    //  最多缓存几条没有成功上传的日志
    //
    static const uint32_t gMaxCacheCount=5;
    
    //
    // 最多连续几次创建writer失败后，不再创建了
    //
    static const uint32_t gMaxCreateWriterFailedCount=5;
    
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
        mCreateWriteNum=0;
		
		if (mOutputWriter.isValid()) {
            if(type == LOG_ST_NEW_FILE || type ==LOG_ST_EXIST_FILE){
                //
                //   文件类型的直接先清空以前的writer
                //
                mWriter=NULL;
            }
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
        LogInfo* info = (LogInfo*)(msg->getPayload());     
		if (!mWriter.isEmpty()) {
			IOWriter* writer=mWriter->getWriter();			
			if (writer) {
                size_t writed = 0;
                
                size_t count=mInfoCache.size();
                for(size_t i=0;i<count && mInfoCache.size()>0;i++){
                    if (FFL_OK != writer->write((void*)(mInfoCache.front().c_str()),
                                                mInfoCache.front().size(), &writed)) {
                       break;
                    }
                    mInfoCache.pop_front();
                }
                
               
                //
                // 成功则退出
                //
                if (FFL_OK == writer->write((void*)(info->mInfo.c_str()),
                                            info->mInfo.size(), &writed)) {
                    return;
                }
				
			}
		}
        
        
        if(mInfoCache.size()>=gMaxCacheCount){
            mInfoCache.pop_front();
        }
        mInfoCache.push_back(info->mInfo);
        
        
		tryCreateWriter();
	}

	//
	//  更新writer
	//
	void LogUploader::handleUpdateWriter(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		LogWriterMessage* payload=msg->getPayloadT<LogWriterMessage>();
		mWriter = payload->mWriter;
        if(mWriter!=NULL){
        }
	}


	void LogUploader::tryCreateWriter() {
        if(mCreateWriteNum>=gMaxCreateWriterFailedCount){
           return;
        }
        
		if (mCreateWriterTime == 0 ||
			FFL_getNowUs() - mCreateWriterTime > (1000 * 1000) * 60 ) {
			//
			//  是否跟上一次大于一分钟了
			//
			createWriter();
            mCreateWriteNum++;
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


