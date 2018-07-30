/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NodeBase.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/29 
*  https://github.com/zhenfei2016/FFL-v2.git
*  Pipeline的一些公用函数，结构等
*
*/
#ifndef _FFL_NODE_BASE_HPP_
#define _FFL_NODE_BASE_HPP_

#include <pipeline/FFL_Pipeline.hpp>
#include <utils/FFL_String.hpp>

namespace FFL {		
		//
		//  Node的输入接口
		//
		class InputInterface {
			friend class NodeBase;
		public:
			InputInterface();
			InputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineInputId inputId);
			//
			// 重置数据
			//
			void reset();
			//
			// 是否有效
			//
			bool isValid() const;
			//
			// 获取所属的Nodeid和接口的id
			//
			FFL::PipelineNodeId getNodeId() const;
			FFL::PipelineInputId getId() const;
			const char* getName();
		private:
			FFL::PipelineNodeId  mNodeId;
			FFL::PipelineInputId mId;
			FFL::String mName;
		};
		//
		//  就是Node的输出接口
		//
		class OutputInterface {
			friend class NodeBase;
		public:
			OutputInterface();
			OutputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineOutputId outputId);
			//
			// 重置数据
			//
			void reset();
			//
			// 是否有效
			//
			bool isValid() const;
			//
			// 获取所属的Nodeid和接口的id
			//
			FFL::PipelineNodeId getNodeId() const;
			FFL::PipelineOutputId getId() const;
		private:
			FFL::PipelineNodeId  mNodeId;
			FFL::PipelineOutputId mId;
			FFL::String mName;
		};

		//
		//  管理node
        //
		class NodeManager {
		public:
			NodeManager(FFL::sp<FFL::Pipeline> pipeline);
			virtual ~NodeManager();
		public:
			//
			// 保存node
			//
			void registerNode(FFL::sp<NodeBase> node);
			void unRegisterNode(FFL::sp<NodeBase> node);

			FFL::Pipeline* getPipeline();
		private:
			//
			//  节点管理
			//
			FFL::CMutex mNodeLock;
			FFL::List< FFL::sp<NodeBase> > mNodeList;

			FFL::sp<FFL::Pipeline> mPipeline;
		};
		//
		//  node的基类
		//
		class NodeBase : public FFL::RefBase{
		public:
			NodeBase();
			virtual ~NodeBase();
			//
			//  获取设置名称
			//
			void setName(const char* name);
			void getName(FFL::String& name) const;
			const char* getName();
			//
			// 开始创建这个节点
			//
			status_t create(NodeManager* mgr);
			void destroy();
			bool isCreated() const;
			//
			//  获取这个node的所有者player
			//
			NodeManager* getManager() const;
			//
			//  获取这个节点属于哪一个线的
			//
			FFL::sp<FFL::Pipeline> getPipeline();
			//
			//  获取nodeid
			//
			FFL::PipelineNodeId getNodeId() const;

			/////////////////////////////////////////////////////////////////////////////////////////////
			//    输入相关，其他节点输入的数据在这些输入接口部分处理
			/////////////////////////////////////////////////////////////////////////////////////////////
			//
			//  获取输入
			//
			FFL::sp<FFL::PipelineInput> getInput(FFL::PipelineInputId id);
			FFL::sp<FFL::PipelineConnector> getConnector(FFL::PipelineInputId id);
			//
			//  创建输入接口
			//
			InputInterface createInputInterface(
				   FFL::sp<FFL::PipelineInputHandler> callback,
				   const char* name);
			//
			//  output:数据提供者输出接口
			//	input：返回当前的这个节点的输入接口接口
			//  返回是否成功
			//
			InputInterface  connectInput(
				   const OutputInterface& input,
				   const char* name,
				   void* userdata);
		
			InputInterface  connectTimerInput(				   
				   const char* name,
				   uint32_t timeMs,
				   void* userdata);
			/////////////////////////////////////////////////////////////////////////////////////////////
			//    输出相关，本节点输出的数据
			/////////////////////////////////////////////////////////////////////////////////////////////
			FFL::sp<FFL::PipelineOutput> getOutput(FFL::PipelineOutputId id);			
			//FFL::sp<FFL::PipelineConnector> getConnector(FFL::PipelineOutputId id);			
			//
			//  创建输出接口
			//
			OutputInterface createOutputInterface();		
			OutputInterface connectOutput(const InputInterface& output);
			//
			//   清空输出节点中没有处理的消息
			//
			void clearMessage(FFL::PipelineInputId dstId);
		private:
			//
			//  连接起来一个输出，一个输入
			//
			bool connect(const OutputInterface& output,
				const InputInterface& input,
				FFL::sp<FFL::PipelineConnector> conn);

		protected:
			friend class FFL::ClassMethodPipelineInputHandler<NodeBase>;
			//
			// 收到消息 ,connectSource建立的连接，连接上的数据来了会触发这个函数
			//
			void receiveData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
			//
			//   外部connectSource时候调用此函数，创建对应conn
			//
			virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(
				const OutputInterface& output,
				const InputInterface& input,
				void* userdata);						
		public:
			//
			//  发送事件消息
			//
			void postEvent(const FFL::sp<FFL::PipelineEvent> &msg);
			//
			//  发送消息到output		
			//  dstId: 目标id
			//  msg : 消息内容
			//
			status_t postMessage(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg);
			status_t postMessageDelay(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs);
			//
			//  给当前节点的一个输入接口发送一条消息
			//
			status_t postSelfMessage(FFL::PipelineInputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs);
		protected:

		protected:
			//
			//  成功创建了node
			//
			virtual void onCreate();
			//
			//  成功删除了node
			//
			virtual void onDestroy();
			virtual void onStartMessageLooper();
			virtual void onQuitMessageLooper();			

			virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
		private:
			//
			// 节点id
			//
			FFL::PipelineNodeId mNodeId;
			//
			// 节点的名称
			//
			FFL::String mNodeName;
			//
			//  pipeline系统中的处理句柄
			//
			class NodeHandler;
			FFL::sp<NodeHandler> mNodeHandler;
			
			//
			//  所有的连接器
			//
			FFL::List< FFL::sp<FFL::PipelineConnector> > mConns;
			//
			//  所属的播放器
			//
			NodeManager* mNodeMgr;
		};
	
}

#endif