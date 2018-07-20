/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineNodeManager.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/02/11
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  Pipelin系统中的Node节点管理器，添加删除等
*
*/

#include "FFL_PipelineNodeManager.hpp"
#include <pipeline/FFL_PipelineConnector.hpp>
#include <pipeline/FFL_PipelineNodeHandler.hpp>

namespace FFL {
	//
	//  pipeline: manager所有者
	//  用户需要保证pipeline删除前把这个manager干掉
	//
	PipelineNodeManager::PipelineNodeManager(Pipeline* pipeline):
		mPipeline(pipeline),
		mNextNodeId(1){

		CMutex::Autolock l(mNodeVectorMutex);
		for (int i = 0; i < NODEVECTOR_SIZE; i++) {
			mNodeVector[i].node=NULL;
			mNodeVector[i].handler = NULL;
		}
	}
	PipelineNodeManager::~PipelineNodeManager() {
		destroyAllNode();
	}
	//
	// 创建一个Node节点
	// handler： 为节点的处理逻辑句柄
	// 返回Node节点的 id  , PipelineNode::INVALID_NodeId 表示无效值
	//
	PipelineNode::NodeId  PipelineNodeManager::createNode(sp<PipelineNodeHandler> handler) {
		if (handler.is_empty()) {
			return PipelineNode::INVALID_NodeId;
		}

		NodeInfo* nodeInfo = 0;
		PipelineNodeId nodeId = 0;
		{
			CMutex::Autolock l(mNodeVectorMutex);
			nodeId=mNextNodeId++;
			int index = NodeId2NodeInfoIndex(nodeId);
			nodeInfo = &mNodeVector[index];
		}

		PipelineNode* node= new PipelineNode(mPipeline, handler, nodeId);
		nodeInfo->node = node;
		nodeInfo->handler = handler;
		return node->getId();
	}
	//
	//删除一个Node节点
	//
	void PipelineNodeManager::destroyNode(PipelineNode::NodeId nodeId) {
		uint32_t index = NodeId2NodeInfoIndex(nodeId);

		PipelineNode* node = NULL;
		CMutex::Autolock l(mNodeVectorMutex);
		if (index < mNextNodeId) {			
			node = mNodeVector[index].node;
			mNodeVector[index].node = 0;
			mNodeVector[index].handler = 0;
		}

		if (node) {
			delete node;
		}
	}
	//
	//删除所有的节点
	//
	void PipelineNodeManager::destroyAllNode() {
		
		PipelineNode* node = NULL;
		CMutex::Autolock l(mNodeVectorMutex);
		for (int i = 0; i < NODEVECTOR_SIZE; i++) {
			node = mNodeVector[i].node;
			mNodeVector[i].node = 0;
			mNodeVector[i].handler = 0;

			if (node) {
				delete node;
			}
		}
		mNextNodeId = 1;
	}
	//
	// 查找一个节点
	//
	PipelineNode* PipelineNodeManager::findNode(PipelineNode::NodeId nodeId) {
		uint32_t index = NodeId2NodeInfoIndex(nodeId);
		return getNode(index);
	}
	int PipelineNodeManager::NodeId2NodeInfoIndex(PipelineNode::NodeId nodeId) {
		return nodeId - 1;
	}	

	//
	// 获取节点的数量		
	//
	uint32_t PipelineNodeManager::getCount() {
		return mNextNodeId - 1;
	}
	//
	// 通过索引获取节点		
	//
	PipelineNode* PipelineNodeManager::getNode(uint32_t index) {
		CMutex::Autolock l(mNodeVectorMutex);
		if (index < mNextNodeId) {
			return mNodeVector[index].node;
		}
		return NULL;
	}
}
