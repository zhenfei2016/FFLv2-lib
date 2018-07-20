/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineInput.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2017/12/10 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  node的输入接口
 *
*/
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>

#include <utils/FFL_Handler.hpp>
#include <pipeline/FFL_PipelineConnector.hpp>
#include <pipeline/FFL_PipelineInputHandler.hpp>
#include "PipelineLooper.hpp"
#include "PipelineMessageTrackFile.hpp"

namespace FFL {
	PipelineInput::PipelineInput(PipelineNode* pipelineNode, sp<PipelineInputHandler> handler, PipelineInputId id)
		: mId(id),mPipelineNode(pipelineNode)
	{        
		FFL_LOG_DEBUG("PipelineInput::PipelineInput inputid=%d:%d this=%p", mPipelineNode->getId(),getId(), this);
		mInputHandler = handler;	
		mFlags.resetFlags(0);
		mName = "";
	}

	PipelineInput::~PipelineInput()
	{		
		mInputHandler.clear();
		FFL_LOG_DEBUG("PipelineInput::~PipelineInput inputid=%d:%d this=%p", mPipelineNode->getId(), getId(), this);
	}

	void PipelineInput::setId(PipelineInputId id) { 
		mId = id; }
	void PipelineInput::setName(const char* name) { 
		mName = (name!=NULL)?name:""; }

	//
	//  获取这个Input在PipelineNode中的唯一id,
	//
	PipelineInputId PipelineInput::getId() const
	{
		return mId;
	}

	void PipelineInput::getName(String& name) const
	{
		name = mName;
	}
	//
	//  连接输出端到这个上面
	//  conn   ：output与这个input是如何连接的
	//  output ：这是数据的输入节点
	//
	void PipelineInput::connect(sp<PipelineConnector> conn)
	{		
		if (conn.isEmpty()) {
			FFL_LOG_WARNING("PipelineInput connect fail. conn is null");
			return;
		}

		CMutex::Autolock l(mLock);
		if (mFlags.getFlags() != 0) {
			FFL_LOG_WARNING("PipelineInput not init.");
			return;
		}		

		mConnector = conn;		
		mConnector->setInput(this);
		mFlags.modifyFlags(CONNECTED,0);
	}
	//
	// 断掉这个连接
	//
	void PipelineInput::disconnect() {
		CMutex::Autolock l(mLock);
		if (mFlags.getFlags() == CONNECTED) {
			if (!mConnector.isEmpty()) {
				mConnector->setInput(NULL);
				mConnector = NULL;
			}
			mFlags.modifyFlags(0, CONNECTED);
		}
	}
	//
	// 启动\停止 输入
	//
	status_t PipelineInput::startup()
	{	
		{
			CMutex::Autolock l(mLock);
			if (mFlags.hasFlags(SHUTDOWNING)) {
				FFL_LOG_WARNING("PipelineInput::startup fail. shutdowning");
				return FFL_ERROR_FAIL;
			}else if (!mFlags.hasFlags(CONNECTED)) {
				FFL_LOG_WARNING("PipelineInput::startup fail. need conn");
				return FFL_ERROR_FAIL;
			}else if (mFlags.hasFlags(STARTING | LOOPING)) {
				FFL_LOG_WARNING("PipelineInput::startup already start.");
				return FFL_OK;
			}
			mFlags.modifyFlags(STARTING, 0);
	    }

		bool started = true;;
		if (FFL_OK != mConnector->startup()) {
			FFL_LOG_ERROR("PipelineInput::startup fail");
			started = false;
		}


		{
			CMutex::Autolock l(mLock);
			mFlags.modifyFlags(LOOPING, started? STARTING : 0);
		}
		
		if (started) {
			//发送启动成功消息
			postStartSuccessMessage();
			return FFL_OK;
		}

		return FFL_FAILED;
	}


	void PipelineInput::postStartSuccessMessage() {
		//
		//  发送这个input接口的第一次启动消息
		//
		sp<PipelineMessage> startLooperMsg = createSysPipelineMessage((uint32_t)MSGT_SYS_START_LOOPER);
		mConnector->tranport(startLooperMsg, 0);
	}
	status_t PipelineInput::shutdown()
	{
		requestShutdown();
		waitShutdown();
		return FFL_OK;
	}

	// 
	// 异步请求shutdown
	//
	status_t PipelineInput::requestShutdown() {

		{
			CMutex::Autolock l(mLock);
			if (!mFlags.hasFlags(LOOPING)) {
				FFL_LOG_WARNING("PipelineInput::shutdown fail.");
				return FFL_ERROR_FAIL;
			}
			mFlags.modifyFlags(SHUTDOWNING, 0);
		}

		mConnector->requestShutdown();		
		return FFL_OK;
	}
	//
	//  请求shutdown并且等待shutdown完成了，会同步等待的
	//
	status_t PipelineInput::waitShutdown() {

		{
			CMutex::Autolock l(mLock);
			if (!mFlags.hasFlags(SHUTDOWNING)) {
				FFL_LOG_WARNING("PipelineInput::waitShutdown fail.");
				return FFL_ERROR_FAIL;
			}			
		}
		mConnector->waitShutdown();
		{
			CMutex::Autolock l(mLock);			
			mFlags.modifyFlags(0, SHUTDOWNING | LOOPING);
		}
		
		return FFL_OK;
	}
	bool PipelineInput::isStarted() {
		CMutex::Autolock l(mLock);
		return mFlags.hasFlags(LOOPING|STARTING);
	}
	//
	//  发送消息给自己
	//
	status_t PipelineInput::postSelfMessage(const sp<PipelineMessage>& msg, int64_t delayUs) {
		sp<PipelineConnector> conn;
		{
			CMutex::Autolock l(mLock);
			sp<PipelineConnector> conn = mConnector;
		}

		if (!conn.isEmpty()) {
			return conn->tranport(msg,delayUs);
		}	

		return FFL_ERROR_FAIL;
	}
	//
	//  清空等待处理的消息
	//
	void PipelineInput::clearMessage() {
		sp<PipelineConnector> conn;
		{
			CMutex::Autolock l(mLock);
			conn = mConnector;
		}

		if (!conn.isEmpty()) {
			conn->clearMessage();
		}
	}

    //
    // 分派系统消息，马上执行的， post的消息最终也会走这地方的
    //
    void PipelineInput::dispathSysMessage(const sp<PipelineMessage>& msg)
    {
        //
        //  退出消息循环
        //
        if (msg.isEmpty()) {
            mPipelineNode->handleQuitMessageLooper(getId(),0);
            return;
        }
        
        if(msg->getType()==MSGT_SYS_START_LOOPER){
            mPipelineNode->handleStartMessageLooper(getId());         
        }

		sp<PipelineConnector> conn = mConnector;
		if (!conn.isEmpty()) {
			conn->consumer(msg);
		}
    }

	//
	// 分派消息，马上执行的
	//
	void PipelineInput::dispathMessage(const sp<PipelineMessage>& msg)
	{	
		int64_t trackId = msg->trackId();
		if (trackId != -1) {
			msg->trackStat("id:%" lld64  ",node:%s,tid:%d,procBeginTime:%" lld64,
				trackId, mName.c_str(), FFL_CurrentThreadID(), FFL_getNowUs());
		}
		//
		//  其他消息的处理
		//
		if (!mInputHandler.isEmpty() && 
			mPipelineNode->prepareHandleMessage(getId(),msg)) {
			mInputHandler->handleMessage(msg);
		}else {
			
		}

		if (trackId != -1) {
			msg->trackStat("procEndTime:%" lld64, FFL_getNowUs());			
			//
			//  打印track信息
			//
			String info;
			msg->trackInfo(info);
			postTraceinfo(info);
			msg->trackIdReset(trackId);
		}

		sp<PipelineConnector> conn = mConnector;
		if (!conn.isEmpty()) {
			conn->consumer(msg);
		}
	}
}
