/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineNodeManager.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/02/11
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  Pipelin系统中的Node节点管理器，添加删除等
*
*/

#ifndef _FFL_PIPELINENODE_MANAGER_HPP_
#define _FFL_PIPELINENODE_MANAGER_HPP_

#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineOutput.hpp>
#include <pipeline/FFL_Pipeline.hpp>

namespace FFL{
	class Pipeline;
	class PipelineNodeHandler;

	class PipelineNodeManager { 	
		friend class Pipeline;
	protected:
		//
		//  pipeline: manager所有者
		//  用户需要保证pipeline删除前把这个manager干掉
		//
		PipelineNodeManager(Pipeline* pipeline);
		~PipelineNodeManager();
	public:
		//
		// 创建一个Node节点
		// handler： 为节点的处理逻辑句柄
		// 返回Node节点的 id  , PipelineNode::INVALID_NodeId 表示无效值
		//
		PipelineNode::NodeId  createNode(sp<PipelineNodeHandler> handler);
		//
		//删除一个Node节点
		//
		void destroyNode(PipelineNode::NodeId nodeId);
		//
		//删除所有的节点
		//
		void destroyAllNode();
		//
		// 查找一个节点
		//
		PipelineNode* findNode(PipelineNode::NodeId nodeId);
		//
		// 获取节点的数量		
		//
		uint32_t getCount();
		//
		// 通过索引获取节点		
		//
		PipelineNode* getNode(uint32_t index);
	protected:
		//
		// Pipeline管理的所有的PipelineNode
		//
		struct NodeInfo {
			PipelineNode* node;
			wp<PipelineNodeHandler> handler;
		};	
	protected:
		//
		// 当前manager所有者
		//
		Pipeline* mPipeline;

		enum {
			NODEVECTOR_SIZE = 20,
		};
		int NodeId2NodeInfoIndex(PipelineNode::NodeId nodeId);
		CMutex mNodeVectorMutex;
		NodeInfo mNodeVector[NODEVECTOR_SIZE];
		PipelineNode::NodeId mNextNodeId;
	};
}


#endif
