/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineMessage.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/23
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  pipelineMessage消息
*
*/

#include <pipeline/FFL_PipelineMessage.hpp>
#include <utils/FFL_Message.hpp>

#define ENABLE_TRACKBACK 

namespace FFL {

	class TrackInfo {
	public:
		TrackInfo() {

		}
		void clear() {
			mInfos = "";
		}

		void add(const char* format, va_list args) {
			char str[1024] = "";			
			vsnprintf(str, 1024 - 1, format, args);
			if (mInfos.size() > 0) {
				mInfos.append(",");
			}
			mInfos.append(str);
		}

		void toString(String& info) {			
			info = mInfos;
		}

		String mInfos;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PipelineMessage::PipelineMessage() :
		mType(0),
		mPayload(NULL),
		mConsumeListener(NULL)
	{
		mMessageUniqueId = 0;
        mPayloadAutoDel=false;	
		mParam1 = 0;
		mParam2 = 0;

		mTrackInfo = new TrackInfo();
		trackIdReset(-1);
	}
	//
	//  消息类型
	//
	PipelineMessage::PipelineMessage(uint32_t msgType) 	:
		mType(msgType),
		mPayload(0) ,
        mConsumeListener(NULL)
	{
        mPayloadAutoDel=false;
        FFL_LOG_DEBUG("PipelineMessage::PipelineMessage(%d) %p",mType,this);

		mTrackInfo = new TrackInfo();
		trackIdReset(-1);
	}
    
    PipelineMessage::~PipelineMessage(){
         FFL_LOG_DEBUG("PipelineMessage::~PipelineMessage(%d) %p",mType,this);
        if(mPayloadAutoDel && mPayload){
            delete mPayload;
            mPayload=NULL;
        }

		FFL_SafeFree(mTrackInfo);
    }
	//
	//  消息类型
	//
	uint32_t PipelineMessage::getType() const 
	{
		return mType;
	}
	void PipelineMessage::setType(uint32_t type)
	{
		mType = type;
	}
	//
	//  这个消息消耗监听器
	//  当消息被消耗完的时候会执行对应listener
	//
	void PipelineMessage::setConsumeListener(MessageConsumeListener* listener)
	{
		mConsumeListener = listener;
	}
	//
	//  这个消息已经被消耗了
	//  消息正常的被handler处理后触发consume的调用
	//  消息cancel也会触发consume
	//
	void PipelineMessage::consume(void*){
		mMessageUniqueId = 0;		
		if (mPayload) {			
			mPayload->consume();
		}

		//
		//  打印track信息
		//
		if (trackId() != -1) {			
			trackStat("consume:%" lld64,FFL_getNowUs());
		}

		//
		//  通知监听器
		//
		if (mConsumeListener) {
			mConsumeListener->onConsume(this);
		}
	}      

	//
	//   设置负载信息
	//
	void PipelineMessage::setPayload(PipelineMessagePayload* payload,bool autoDel) {        
        if(mPayloadAutoDel && mPayload && mPayload !=payload){
            delete mPayload;
            mPayload=NULL;
            
        }
		mPayload = payload;
        mPayloadAutoDel=autoDel;
	}
	PipelineMessagePayload* PipelineMessage::getPayload() const
	{
		return mPayload;
	}
	//
	// 设置这个消息带的参数，
	//
	void PipelineMessage::setParams(int64_t param1, int64_t param2) {
		mParam1 = param1;
		mParam2 = param2;
	}
	int64_t PipelineMessage::getParam1() const {
		return mParam1;
	}
	int64_t PipelineMessage::getParam2() const {
		return mParam2;
	}


	//
	//  重置追踪id,以前保存的信息会清空
	//
	void PipelineMessage::trackIdReset(int64_t id) {
		mTrackId = id;
		if (mTrackInfo) {
			mTrackInfo->clear();
		}
	}
	//
	// 进行一次统计
	//
	void PipelineMessage::trackStat(const char* format, ...) {
		if (mTrackInfo == NULL) {
			return;
		}
		va_list args;
		va_start(args, format);		
		mTrackInfo->add(format,args);
		va_end(args);
	}
	//
	//  获取所有的统计信息
	//
	void PipelineMessage::trackInfo(String& info) {
		if (mTrackInfo) {
			mTrackInfo->toString(info);
		}
	}
	//
	//  通过这个id追踪这个消息的处理过程
	//
	int64_t PipelineMessage::trackId() {
		return mTrackId;
	}   

    //
    //  是否系统消息
    //
    bool isPipelineMessage(sp<Message> msg){
        uint32_t what = msg->what();
        return (what ==MSGT_PIPELINE);
    }
	//
	// 获取pipelineMessage消息
	//
	sp<PipelineMessage> getPipelineMessage(sp<Message> msg) {
		if (msg.isEmpty()) {
			return NULL;
		}

		uint32_t what = msg->what();
		if (what == MSGT_PIPELINE || (what >= MSGT_SYS_BEGIN && what <= MSGT_SYS_END)) {
			sp<PipelineMessage> message = (PipelineMessage*)(msg->mObj.get());
			return message;
		}		
		return NULL;
	}
	//
	//  是否系统消息
	//
	bool isSysPipelineMessage(sp<PipelineMessage> msg) {
		uint32_t what = msg->getType();
		return (what >= MSGT_SYS_BEGIN && what <= MSGT_SYS_END);
	}
    //
    //  创建指定类型的系统消息
    //
    sp<PipelineMessage> createSysPipelineMessage(uint32_t type){
        sp<PipelineMessage> msg=new PipelineMessage(type);
        msg->setType(type);
        return  msg;
    }

}
