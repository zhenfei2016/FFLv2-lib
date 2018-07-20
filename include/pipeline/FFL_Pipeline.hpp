/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Pipeline.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/02 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  流水线实例，可以包含n多的节点 ，然后启动，结束流水线
*
*/

#ifndef _FFL_PIPELINE_HPP_
#define _FFL_PIPELINE_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>
#include <pipeline/FFL_PipelineDefined.hpp>
#include <utils/FFL_Flags.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineEventFilter.hpp>
#include <pipeline/FFL_PipelineNodeHandler.hpp>
#include <pipeline/FFL_PipelineInputHandler.hpp>
#include <pipeline/FFL_PipelineConnector.hpp>
#include <pipeline/FFL_PipelineSourceConnector.hpp>
#include <utils/FFL_String.hpp>

namespace FFL{
	class PipelineNodeManager;
	

	class Pipeline : public RefBase{
	public:		
		//
		//  scrip: pipeline创建的node的脚本
		//
		Pipeline(const char* scrip=NULL);
		~Pipeline();
	private:
		void onFirstRef();
		void onLastStrongRef(const void* id);
	public:
		//
		//  启动pipeline, 
		//  异步的
		//  成功返回 FFL_OK
		//  注意startup后不需要了必须调用shutdown
		//
		status_t startup();		
		//
		//  注意只有在整个pipeline启动后才能调用这函数
		//  同步，异步启动所有的node，	如果node已经启动则会忽略
		//  如果未启动则启动它
		//
		status_t ansyStartupAllNode();
		status_t syncStartupAllNode();
		//
		// 注意只有在整个pipeline启动后才能调用这函数
		// 同步，异步启动一个或多个node，		
		// 成功返回 FFL_OK		
		//
		status_t syncStartupNode(PipelineNodeId nodeId);
		status_t ansyStartupNode(PipelineNodeId nodeId);
		status_t syncStartupNode(PipelineNodeId* nodeId, int32_t count);
		status_t ansyStartupNode(PipelineNodeId* nodeId, int32_t count);

		//
		//  停止pipeline,异步的
		//
		status_t shutdown();	
		//
		//  停止pipeline,并等待成功
		//
		status_t shutdownAndWait();
		//
		//  调用这个以后这个pipeline 就不能使用了
		//
		void exit();
	private:
		//
		//  启动的处理逻辑，
		//
		void handleStartup(const sp<PipelineEvent>& event);
		//
		//  单个node启动的处理逻辑，
		//
		friend class SingleNodeStartupEvent;
		void handleSingleNodeStartup(const sp<PipelineEvent>& event);
		//
		//  ansyStartupAllNode 触发的所有node进行启动
		//
		void handleAllNodeStartup(const sp<PipelineEvent>& event);
		//
		//  停止的处理逻辑
		//
		void handleShutdown(const sp<PipelineEvent>& event);
		
		CMutex mEventMutex;
		sp<PipelineEvent> mEventStartup;
		bool mEventStartupPending;		

		sp<PipelineEvent> mEventShutdown;
		bool mEventShutdownPending;		
	public:
		//
		// 发送事件到事件系统中， 异步的
		//
		status_t postEvent(const sp<PipelineEvent>& msg);
		//
		// 设置事件过滤器 
		//
		void setEventFilter(PipelineEventFilter* filter);
	private:
		friend class PipelineEventLooper;
		//
		// 事件系统消息处理
		//
		void onEvent(const sp<PipelineEvent> &event);	
		//
		// 整个pipeline以及这个下面的Pipenode的事件处理循环
		//
		sp<PipelineEventLooper> mEventLooper;
		//
		// 事件过滤器
		//
		PipelineEventFilter* mEventFilter;
	private:
		//
		// 获取节点管理器，返回这个Pipeline持有的PipelineNode
		// 生命周期与这个Pipeline一致的
		//
		PipelineNodeManager*  getNodeManager();
		PipelineNodeManager* mNodeManager;
	public:
		//
		// 创建一个Node节点
		// handler： 为节点的处理逻辑句柄
		// 返回Node节点的 id  , PipelineNode::INVALID_NodeId 表示无效值
		//
		PipelineNodeId  createNode(sp<PipelineNodeHandler> handler);
		//
		// 创建一个输入接口
		//
		PipelineInputId createInput(PipelineNodeId nodeId, sp<PipelineInputHandler> handler,const char* name);
        //
		// 删除一个输入接口
		//
		void destroyInput(PipelineNodeId nodeId,PipelineInputId id);
		//
		// 创建一个输出接口
		//
		PipelineOutputId createOutput(PipelineNodeId nodeId);
		//
		// 删除一个输出接口
		//
		void destroyOutput(PipelineNodeId nodeId, PipelineOutputId id);
		//
		//删除一个Node节点 ，节点删除时候，所有的输入，输出接口都断掉
		//
		void destroyNode(PipelineNodeId nodeId);
		//
		//连接一个Node的输出口到另一个的输入口
		//conn :连接类型
		//
		status_t connect(
			PipelineNodeId srcNodeId, PipelineOutputId srcOutputId,
			PipelineNodeId dstNodeId, PipelineInputId dstInputId,
			sp<PipelineConnector> conn);

		//
		//  连接一个源节点
		//
		status_t connectSource(PipelineNodeId nodeId, PipelineInputId inputId,
			sp<PipelineSourceConnector> conn);
		//
		//断开输出，输入的连接
		//
		status_t disconnect(PipelineNodeId nodeId, PipelineOutputId srcOutputId);
		//
		//断开nodeid的所有的输入接口
		//
		status_t disconnectInput(PipelineNodeId nodeId);
		//
		//断开nodeid的所有的输出接口
		//
		status_t disconnectOutput(PipelineNodeId nodeId);
	private:
		//
		//Pipeline的状态值
		//
		enum {

			STARTUPING = 0x01,
			LOOPING = 0x02,
			SHUDOWNING = 0x04,
		};		
		Flags32b mFlags;		
	public:
		//
		//  是否工作中
		//
		bool isLooping();

	private:
		CMutex mShutdownMutex;
		CCondition mShutdownCond;

	};
}


#endif


