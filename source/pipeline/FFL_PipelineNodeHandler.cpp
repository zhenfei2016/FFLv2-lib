/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineNodeHandler.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/02/03
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  Pipelin系统中的Node节点的回调句柄
*
*/


#include <pipeline/FFL_PipelineNodeHandler.hpp>
#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_Pipeline.hpp>

namespace FFL
{
	//
	//  发送消息到output		
	//  dstId: 目标id
	//  msg : 消息内容
	//
	status_t PipelineNodeHandler::postMessage(PipelineOutputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs) {
		if(mPipeLineNode)
		   return mPipeLineNode->postMessage(dstId,msg,delayUs);
		return FFL_ERROR_FAIL;
	}

	//
	//  发送消息给自己	
	//  dstId: 目标id
	//  msg : 消息内容
	//
	status_t PipelineNodeHandler::postSelfMessage(PipelineInputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs) {
	
		sp<PipelineInput> input=mPipeLineNode->getInput(dstId);
		if (!input.isEmpty()) {
			//input->get
		}

		return FFL_OK;
	}
    
    sp<PipelineInput> PipelineNodeHandler::getInput(PipelineInputId id){
        return mPipeLineNode->getInput(id);

    }
	sp<PipelineOutput> PipelineNodeHandler::getOutput(PipelineOutputId id) {
		return mPipeLineNode->getOutput(id);
	}
	//  发送事件消息
	//
	void PipelineNodeHandler::postEvent(const sp<PipelineEvent> &msg) {
		sp<Pipeline> pipeline = mPipeLineNode->getPipeline();
		pipeline->postEvent(msg);
	}
}


