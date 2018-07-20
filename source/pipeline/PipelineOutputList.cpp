/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineOutputList.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/16
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  node的输出接口管理
*
*/

#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_PipelineOutput.hpp>
#include "PipelineOutputList.hpp"

namespace FFL {
	PipelineOutputList::PipelineOutputList(PipelineNode* node):mNextOutputId(1)
	{
		mPipelineNode = node;

		OutputInfo info = {};		
		mOutputs.push_back(info);
		mNextOutputId= mOutputs.size();
	}
	PipelineOutputList::~PipelineOutputList(){
	}


	//
	//  创建一个新的input
	//
	PipelineOutputId PipelineOutputList::createOutput(){

		PipelineOutputId id = mNextOutputId;
		PipelineOutput* output = new PipelineOutput(mPipelineNode, id);

		OutputInfo info = {};
		info.output = output;
		mOutputs.push_back(info);
		mNextOutputId = mOutputs.size();
		return id;
	}
	//
	//  删除一个input
	//
	void PipelineOutputList::destroyOutput(PipelineOutputId id){
		if (!isValidId(id)) {
			FFL_LOG_WARNING("destroyInput fail. ");
			return;
		}
		
		OutputInfo& info = mOutputs[id];		
		if (!info.output.isEmpty()) {			
			info.output->disconnect();
			info.output = NULL;
		} 
	}
	//
	//  通过id查找一个input
	//
	sp<PipelineOutput> PipelineOutputList::getOutput(PipelineOutputId id){
		if (!isValidId(id)) {
			FFL_LOG_WARNING("destroyInput fail. ");
			return NULL;
		}

		OutputInfo& info = mOutputs[id];
		if (!info.output.isEmpty()) {				
		    return info.output;
		}
		return NULL;
	}
	//
	//  获取所有的有效的input
	//
	void PipelineOutputList::getOutputVector(Vector< sp<PipelineOutput> >& list) {
		for (Vector<OutputInfo>::iterator it = mOutputs.begin(); it != mOutputs.end(); it++) {
			OutputInfo& info = *it;
			if (info.output.isEmpty()) {
				continue;
			}			
			list.push_back(info.output);
		}
	}

	bool PipelineOutputList::isValidId(PipelineOutputId id) {
		if (mOutputs.size() <= (size_t)id) {
			FFL_LOG_WARNING("invalid input id. ");
			return false;
		}

		return true;
	}
}
