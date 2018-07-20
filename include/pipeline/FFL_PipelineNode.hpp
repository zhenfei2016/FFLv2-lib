/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineNode.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/02/03
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  Pipelin系统中的Node节点
*
*/

#ifndef _FFL_PIPELINENODE_HPP_
#define _FFL_PIPELINENODE_HPP_


#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineOutput.hpp>
#include <utils/FFL_Vector.hpp>


namespace FFL
{
	class Pipeline;
	class PipelineNodeHandler;
	class PipelineInputList;
	class PipelineOutputList;
	class PipelineNode{
	public:
		typedef uint32_t NodeId;
		enum { INVALID_NodeId = 0 };

		//
		//PipelineNode :创建是由PipelineNodeManager进行的
		//
		friend class PipelineNodeManager;
	protected:
		//
		//  pipeline ： 所属的pipeline
		//  handler  :  这个node逻辑处理句柄
		//
		PipelineNode(sp<Pipeline> pipeline, sp<PipelineNodeHandler> handler,NodeId nodeId);
		~PipelineNode();
	public:
		NodeId getId() const;
		//
		//  获取宿主pipeline
		//
		sp<Pipeline> getPipeline();

		//
		// 启动这个Node节点的处理循环. 由pipenode调用
		//  todo :startup后除了消息循环默认式pause状态，需要resume
		//
		status_t startup();
		//
		//停止这个节点的处理. 由pipenode调用
		//
		status_t shutdown();	
		//
		// 暂停，恢复消息的处理
		// todo
		//
		void pause();
		void resume();
		//
		//停止这个节点的一个输入接口处理
		//
		status_t shutdownInput(PipelineInputId id);
	public:
        //
        // 开始消息循环
        //
        void handleStartMessageLooper(PipelineInputId id);
		//
		// 来了一个消息处理前的准备
		// context： 消息上下文信息
		// msg: 消息体
		// 返回表示是否需要处理
		//
		bool prepareHandleMessage(PipelineInputId id, const sp<PipelineMessage> &msg);
        //
        // 退出消息循环
        //
        void handleQuitMessageLooper(PipelineInputId id,uint32_t exitCode);
		//
		//  发送消息到output
		//  srcId: 源id Input  
		//  dstId: 目标id
		//  msg : 消息内容
		//
		status_t postMessage(PipelineOutputId dstId, const sp<PipelineMessage> &msg, uint64_t delayUs);

	public:
		//
		//  事件系统上来的事件
		//
		void onEvent(const sp<PipelineEvent> &msg);
		//
		//  发送事件到事件系统上
		//
		void postEvent(const sp<PipelineEvent> &msg);

	public:		
		//
		// 创建,删除，获取输入接口
		//
		PipelineInputId createInput(const char* name, sp<PipelineInputHandler> handler);
		PipelineInputId createInput(sp<PipelineInputHandler> handler);
		void destroyInput(PipelineInputId id);
		sp<PipelineInput> getInput(PipelineInputId id);
		void getInputVector(Vector< sp<PipelineInput> >& list);
		void getStartedInputVector(Vector< sp<PipelineInput> >& list);
		void getStopedInputVector(Vector< sp<PipelineInput> >& list);

		
		//
		// 创建,删除，获取输出接口
		//
		PipelineOutputId createOutput();
		void destroyOutput(PipelineOutputId id);
		sp<PipelineOutput> getOutput(PipelineOutputId id);
		void getOutputVector(Vector< sp<PipelineOutput> >& list);
	protected:	
		NodeId mId;
	private:
		//
		//  这个node的所有输入，输出接口
		//
		CMutex mMutex;
		PipelineOutputList* mOutputList;
		PipelineInputList* mInputList;

		//
		//  这个就不加锁了，影响不是太大
		//
		Flags32b mFlag;

		enum {
			STARTED = 1,
		};

		//
		//  node的拥有者,那一条流水线下的节点
		//
		Pipeline* mPipeline;
		//
		//  当这个node启动后，保存Pipeline的引用
		//  当这个node结束后，Pipeline的引用自动去除
		//  这个变量其他地方不要引用
		//
		sp<Pipeline> mPipelineRef;
		//
		//  node的处理handler
		//
		wp<PipelineNodeHandler> mHandler;
	};


}


#endif
