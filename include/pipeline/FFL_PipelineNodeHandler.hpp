/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineNodeHandler.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/02/03
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  Pipelin系统中的Node节点的回调句柄
*
*/

#ifndef _FFL_PIPELINENODE_HANDLER_HPP_
#define _FFL_PIPELINENODE_HANDLER_HPP_

#include <ref/FFL_Ref.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineOutput.hpp>

namespace FFL
{
	class PipelineNode;	
	class PipelineNodeHandler :public RefBase {
	public:
		PipelineNodeHandler() :mPipeLineNode(0){}
		virtual ~PipelineNodeHandler(){}

		//
		//  返回false则node不进行启动了
		//
		virtual bool onStartupBefore() { return true; }
		virtual void onStartupAfter() {}

		virtual void onShutdownBefore() {}
		virtual void onShutdownAfter() {}

		virtual void onEvent(const sp<PipelineEvent> &msg) {}

        virtual void onStartMessageLooper(PipelineInputId id){};
		//
		//  消息前处理
		//
		virtual bool prepareHandleMessage(PipelineInputId id, const sp<PipelineMessage> &msg) { return true; }
		//
		//  退出looper的消息
		//
		virtual void onQuitMessageLooper(PipelineInputId id,uint32_t exitCode) {}
		//
		//  发送事件消息
		//
		void postEvent(const sp<PipelineEvent> &msg);
		//
		//  发送消息到output		
		//  dstId: 目标id
		//  msg : 消息内容
		//
		status_t postMessage(PipelineOutputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs);
		//
		//  发送消息给自己	
		//  dstId: 目标id
		//  msg : 消息内容
		//
		status_t postSelfMessage(PipelineInputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs);
        
        sp<PipelineInput> getInput(PipelineInputId id);
		sp<PipelineOutput> getOutput(PipelineOutputId id);
	private:
		friend class PipelineNode;
		PipelineNode* mPipeLineNode;
	};
}


#endif
