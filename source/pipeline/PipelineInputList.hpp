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

#ifndef _FFL_PIPELINEINPUT_LIST_HPP_
#define _FFL_PIPELINEINPUT_LIST_HPP_

#include <pipeline/FFL_PipelineInput.hpp>
#include <utils/FFL_Vector.hpp>

namespace FFL
{
	class Pipeline;
	class PipelineNode;
	class PipelineNodeHandler;

	class PipelineInputList{
	public:
		PipelineInputList(PipelineNode* node);
		~PipelineInputList();	

	public:
		//
		//  创建一个新的input
		//
		PipelineInputId createInput(const char* name, sp<PipelineInputHandler> handler);
		//
		//  删除一个input
		//
		void destroyInput(PipelineInputId id);
		//
		//  通过id查找一个input
		//
		sp<PipelineInput> getInput(PipelineInputId id);
		//
		//  获取所有的有效的input
		//
		enum InutStatus {			
			STARTED=0x1,
			STOPED=0x2,
			ALL = 0x03,
		};
		void getInputVector(Vector< sp<PipelineInput> >& list, InutStatus status=ALL);
	private:
		bool isValidId(PipelineInputId id);
	protected:
		struct  InputInfo
		{
			InputInfo() {
			
			}
			sp<PipelineInput> input;			
		};

		Vector<InputInfo> mInputs;
		PipelineInputId mNextInputId;

		PipelineNode* mPipelineNode;
	};


}


#endif
