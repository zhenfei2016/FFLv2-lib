/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineInputList.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2017/12/16 
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *
 *  保存输入一组接口的包装类型
 *
*/
#include <pipeline/FFL_PipelineNode.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include "PipelineInputList.hpp"

namespace FFL {
	PipelineInputList::PipelineInputList(PipelineNode* node):mNextInputId(1)
	{
		mPipelineNode = node;

		InputInfo info;		
		mInputs.push_back(info);
		mNextInputId= mInputs.size();
	}
	PipelineInputList::~PipelineInputList(){}


	//
	//  创建一个新的input
	//
	PipelineInputId PipelineInputList::createInput(const char* name, sp<PipelineInputHandler> handler){

		PipelineInputId id = mNextInputId;
		PipelineInput* input = new PipelineInput(mPipelineNode, handler, id);
		input->setName(name);

		InputInfo info ;
		info.input = input;
		mInputs.push_back(info);
		mNextInputId = mInputs.size();
		return input->getId();
	}
	//
	//  删除一个input
	//
	void PipelineInputList::destroyInput(PipelineInputId id) {
		if (!isValidId(id)) {
			FFL_LOG_WARNING("destroyInput fail. ");
			return;
		}

		InputInfo& info = mInputs[id];
		if (!info.input.isEmpty()) {
			if (info.input->isStarted()) {
				info.input->shutdown();			
			}
			
			info.input->disconnect();
			info.input = NULL;			
		}	
	}
	//
	//  通过id查找一个input
	//
	sp<PipelineInput> PipelineInputList::getInput(PipelineInputId id){
		if (!isValidId(id)) {
			FFL_LOG_WARNING("destroyInput fail. ");
			return NULL;
		}

		InputInfo& info = mInputs[id];
		if (info.input.isEmpty()) {				
			return NULL;
		    
		}
		return info.input;
	}
	//
	//  获取所有的有效的input
	//
	void PipelineInputList::getInputVector(Vector< sp<PipelineInput> >& list, InutStatus status ) {
		for (Vector<InputInfo>::iterator it = mInputs.begin(); it != mInputs.end(); it++) {
			InputInfo& info = *it;
			if (info.input.isEmpty()) {
				continue;
			}

			if (status == ALL) {
				list.push_back(info.input);
			}else if (status == STARTED) {
				if(info.input->isStarted())
				   list.push_back(info.input);
			}
			else if (status == STOPED) {
				if (!info.input->isStarted())
					list.push_back(info.input);
			}			
		}
	}

	bool PipelineInputList::isValidId(PipelineInputId id) {
		if (mInputs.size() <= (size_t)id) {
			FFL_LOG_WARNING("invalid input id. ");
			return false;
		}

		return true;
	}
}
