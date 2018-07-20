/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineNode.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/02/03
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  Pipelin系统中的Node节点
 *
*/
#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_Pipeline.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineOutput.hpp>
#include <pipeline/FFL_PipelineNodeHandler.hpp>
#include "PipelineInputList.hpp"
#include "PipelineOutputList.hpp"



namespace FFL
{
	PipelineNode::PipelineNode(sp<Pipeline> pipeline, sp<PipelineNodeHandler> handler, NodeId nodeId):
		mId(nodeId),mPipeline(pipeline.get())
	{
		FFL_LOG_DEBUG("PipelineNode::PipelineNode nodeid=%d this=%p",getId(), this);
		mHandler = handler;
		handler->mPipeLineNode = this;
		mInputList = new PipelineInputList(this);
		mOutputList = new PipelineOutputList(this);
		mFlag.resetFlags(0);
		
	}
	PipelineNode::~PipelineNode() {
		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (!handler.is_empty()) {
			handler->mPipeLineNode = 0;
			handler = 0;
		}
		
		FFL_SafeFree(mInputList);
		FFL_SafeFree(mOutputList);

		FFL_LOG_DEBUG("PipelineNode::~PipelineNode nodeid=%d this=%p", getId(), this);		
	}

	PipelineNode::NodeId PipelineNode::getId() const {
		return mId;
	}
	//
	//  获取宿主pipeline
	//
	sp<Pipeline> PipelineNode::getPipeline() {
		return mPipeline;
	}
	//
	// 启动这个Node节点的处理循环
	//
	status_t PipelineNode::startup()
	{		
		bool bNotifyHandler = !mFlag.hasFlags(STARTED);
		mFlag.resetFlags(STARTED);

		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (bNotifyHandler) {
			if (!handler.is_empty() && handler->onStartupBefore() == false) {
				return FFL_ERROR_FAIL;
			}
		}
		//
		//  获取出还没有启动的input
		//
		Vector< sp<PipelineInput> > inputs;		
		getStopedInputVector(inputs);
		
		//
		//  启动所有的输入节点
		//
		if (inputs.size() == 0) {
			FFL_LOG_WARNING("PipelineNode(%d) input is empty.", getId());
		}
		else {
			for (Vector< sp<PipelineInput> >::iterator it = inputs.begin(); it != inputs.end(); it++) {
				sp<PipelineInput> input = *it;
				if (input.is_empty()) continue;
				if (FFL_OK != input->startup()){
					FFL_LOG_WARNING("PipelineNode.input(%d.%d) startup fail.", getId(), input->getId());
				}			
			}
		}		

		if (bNotifyHandler && !handler.is_empty()) {
			handler->onStartupAfter();
		}
		
		mPipelineRef = mPipeline;
		
		return FFL_OK;
	}

	//
	//停止这个节点的处理
	//
	status_t PipelineNode::shutdown() 
	{		
		mFlag.resetFlags(0);

		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (!handler.is_empty()) {
			handler->onShutdownBefore();
		}

		Vector< sp<PipelineInput> > inputs;
		getStartedInputVector(inputs);

		for (uint32_t i =0; i<inputs.size(); i++) {
			sp<PipelineInput> input = inputs[i];
			if (input.is_empty()) continue;

			if (FFL_OK != input->requestShutdown())
			{
				FFL_LOG_WARNING("PipelineNode.input(%d.%d) startup fail.", getId(), input->getId());
			}			
		}


		for (uint32_t i = 0; i < inputs.size(); i++) {
			sp<PipelineInput> input = inputs[i];
			if (input.is_empty()) continue;

			if (FFL_OK != input->waitShutdown())
			{
				FFL_LOG_WARNING("PipelineNode.input(%d.%d) startup fail.", getId(), input->getId());
			}
		}

		inputs.clear();
		if (!handler.is_empty()) {
			handler->onShutdownAfter();
		}
		mPipelineRef = NULL;

		return FFL_OK;		
	}
	//
	// 暂停，恢复消息的处理
	//
	void PipelineNode::pause() {

	}
	void PipelineNode::resume() {

	}
	//
	//停止这个节点的一个输入接口处理
	//
	status_t PipelineNode::shutdownInput(PipelineInputId id) {
		Vector< sp<PipelineInput> > inputs;
		getStartedInputVector(inputs);

		for (uint32_t i = 0; i < inputs.size(); i++) {
			sp<PipelineInput> input = inputs[i];
			if (input.is_empty()) continue;
			if(input->getId() !=id) continue;

			if (FFL_OK != input->shutdown())
			{
				FFL_LOG_WARNING("PipelineNode.input(%d.%d) startup fail.", getId(), input->getId());
			}
			break;
		}

		return FFL_OK;
	}
    //
    // 开始消息循环
    //
    void PipelineNode::handleStartMessageLooper(PipelineInputId id){
        sp<PipelineNodeHandler>  handler = mHandler.promote();
        if (!handler.is_empty()) {
            return handler->onStartMessageLooper(id);
        }
        else {
            FFL_LOG_WARNING("PipelineNode::handleStartMessageLooper InputId=%d  handler is free.", getId());
        }
    }
	//
	// 来了一个消息处理前的准备
	// context： 消息上下文信息
	// msg: 消息体
	// 返回表示是否需要处理
	//
	bool PipelineNode::prepareHandleMessage(PipelineInputId id, const sp<PipelineMessage> &msg)
	{
		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (!handler.is_empty()) {
			return handler->prepareHandleMessage(id, msg);
		}
		else {
			FFL_LOG_WARNING("PipelineNode::prepareHandleMessage InputId=%d  handler is free.", getId());
		}

		return false;			 
	}
	//
	//  处理退出消息
	//
	void PipelineNode::handleQuitMessageLooper(PipelineInputId id, uint32_t exitCode) {
		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (!handler.is_empty()) {
			return handler->onQuitMessageLooper(id,exitCode);
		}
		else {
			FFL_LOG_WARNING("PipelineNode::handleQuitMessage InputId=%d  handler is free.", getId());
        }
	}

	//
	//  发送消息到output
	//  srcId: 源id Input  
	//  dstId: 目标id
	//  msg : 消息内容
	//
	status_t PipelineNode::postMessage(PipelineOutputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs)
	{
		sp<PipelineOutput> output = getOutput(dstId);
		if (!output.is_empty()) {
			return output->postMessage(msg,delayUs);
		}
		else {
			FFL_LOG_WARNING("PipelineNode().postMessage fail. output(%d) is null",getId(),dstId);
		}
		return FFL_ERROR_FAIL;
	}

	//
	//  事件系统上来的事件
	//
 	void PipelineNode::onEvent(const sp<PipelineEvent> &msg) {
		sp<PipelineNodeHandler>  handler = mHandler.promote();
		if (!handler.is_empty()) {
			handler->onEvent(msg);
		}
		else {
			FFL_LOG_WARNING("PipelineNode::onEvent handler is free.");
		}
	}
	//
	//  发送事件到事件系统上
	//
	void PipelineNode::postEvent(const sp<PipelineEvent> &event) {
		getPipeline()->postEvent(event);
	}
	//
	// 创建一个输入接口,现在的这个handler需要new出来，内部使用完以后会把他删除掉的
	//
	PipelineInputId PipelineNode::createInput(const char* name, sp<PipelineInputHandler> handler)
	{
		CMutex::Autolock l(mMutex);
		return mInputList->createInput(name,handler);
	}
	PipelineInputId PipelineNode::createInput(sp<PipelineInputHandler> handler) {
		return createInput("", handler);
	}

	void PipelineNode::destroyInput(PipelineInputId id) {
		CMutex::Autolock l(mMutex);
		return mInputList->destroyInput(id);
	}

	sp<PipelineInput> PipelineNode::getInput(PipelineInputId id) {
		CMutex::Autolock l(mMutex);
		return mInputList->getInput(id);
	}
	void PipelineNode::getInputVector(Vector< sp<PipelineInput> >& list) {
		CMutex::Autolock l(mMutex);
		return mInputList->getInputVector(list,PipelineInputList::ALL);
	}
	void PipelineNode::getStartedInputVector(Vector< sp<PipelineInput> >& list) {
		CMutex::Autolock l(mMutex);
		return mInputList->getInputVector(list, PipelineInputList::STARTED);
	}
	void PipelineNode::getStopedInputVector(Vector< sp<PipelineInput> >& list) {
		CMutex::Autolock l(mMutex);
		return mInputList->getInputVector(list, PipelineInputList::STOPED);
	}
	
	//
	// 创建一个输出接口
	//
	PipelineOutputId PipelineNode::createOutput() {
		CMutex::Autolock l(mMutex);
		return mOutputList->createOutput();
	}
	void PipelineNode::destroyOutput(PipelineOutputId id) {
		CMutex::Autolock l(mMutex);
		return mOutputList->destroyOutput(id);
	}
	sp<PipelineOutput> PipelineNode::getOutput(PipelineOutputId id) {
		CMutex::Autolock l(mMutex);
		return mOutputList->getOutput(id);
	}
	void PipelineNode::getOutputVector(Vector< sp<PipelineOutput> >& list)
	{
		CMutex::Autolock l(mMutex);
		mOutputList->getOutputVector(list);
	}
}

