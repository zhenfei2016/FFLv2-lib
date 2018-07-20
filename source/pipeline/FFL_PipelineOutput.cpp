/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineOutput.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/16
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  node的输出接口
*
*/

#include <pipeline/FFL_PipelineOutput.hpp>
#include <pipeline/FFL_PipelineConnector.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>
#include <pipeline/FFL_PipelineNode.hpp>


namespace FFL
{	
	PipelineOutput::PipelineOutput(PipelineNode* node, PipelineOutputId id):
        mId(id),
		mPipelineNode(node)		
	{
		FFL_ASSERT(node);
		FFL_LOG_DEBUG("PipelineOutput::PipelineOutput outputId=%d:%d this=%p", mPipelineNode->getId(), getId(), this);		
		mFlags.resetFlags(0);
	}
	PipelineOutput::~PipelineOutput(){
		FFL_LOG_DEBUG("PipelineOutput::~PipelineOutput outputId=%d:%d this=%p", mPipelineNode->getId(), getId(), this);
	}

	PipelineOutputId PipelineOutput::getId() const
	{
		return mId;
	}
	//
	//  连接输出端到这个上面		
	//
	void PipelineOutput::connect(sp<PipelineConnector> conn)
	{
		if (conn.isEmpty()) {
			return;
		}

		CMutex::Autolock l(mLock);
		if (mFlags.hasFlags(CONNECTED)) {
			FFL_LOG_WARNING("connect: PipelineOutput has connected. %d:%d ",mPipelineNode->getId(),getId());
			return;
		}	
		
		mConnector = conn;		
		mConnector->setOutput(this);
		mFlags.resetFlags(CONNECTED);
	}
	//
	// 断掉这个连接
	//
	void PipelineOutput::disconnect() 
	{
		CMutex::Autolock l(mLock);
		if (!mFlags.hasFlags(CONNECTED)) {
			FFL_LOG_WARNING("disconnect: PipelineOutput not connected. %d:%d ", mPipelineNode->getId(), getId());
			return;
		}

		if (!mConnector.isEmpty()) {
			mConnector->setOutput(NULL);
		}
		mConnector = NULL;
		mFlags.modifyFlags(0,CONNECTED);
	}
	//
	// post消息 , 	
	// msg :消息值
	//
	status_t PipelineOutput::postMessage(const sp<PipelineMessage> &msg, uint64_t delayUs) {
		if (mConnector.isEmpty()) {
			return FFL_ERROR_FAIL;
		}

		sp<PipelineConnector> conn;
		{
			CMutex::Autolock l(mLock);
			if (!mFlags.hasFlags(CONNECTED)) {
				FFL_LOG_WARNING("PipelineOutput not connected.");
				return FFL_ERROR_FAIL;
			}

			if (!mConnector.isEmpty() && mConnector->isConnected()) {
				conn = mConnector;
			}else {
				FFL_LOG_WARNING("PipelineOutput disconnected.");
				return FFL_ERROR_FAIL;
			}
		}
		

		if (msg->trackId() != -1) {
			msg->trackStat("id:%" lld64 ",postTime:%" lld64, msg->trackId(),FFL_getNowUs());
		}
		return conn->tranport(msg,delayUs);
	}

	//
	//  清空等待处理的消息
	//
	void PipelineOutput::clearMessage() {
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
	// 获取待处理的消息的大小
	//
	int64_t PipelineOutput::getPendingMessageSize() {
		return 0;
	}
}

