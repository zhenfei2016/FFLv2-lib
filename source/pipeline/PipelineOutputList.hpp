/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineOutputList.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/16
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  node的输出接口管理
*
*/
#ifndef _FFL_PIPELINE_OUTPUT_LIST_HPP_
#define _FFL_PIPELINE_OUTPUT_LIST_HPP_

#include <pipeline/FFL_PipelineOutput.hpp>
#include <utils/FFL_Vector.hpp>

namespace FFL
{
	class Pipeline;
	class PipelineNode;
	class PipelineNodeHandler;

	class PipelineOutputList{
	public:
		PipelineOutputList(PipelineNode* node);
		~PipelineOutputList();

	public:
		//
		//  创建一个新的output
		//
		PipelineOutputId createOutput();
		//
		//  删除一个output
		//
		void destroyOutput(PipelineOutputId id);
		//
		//  通过id查找一个input
		//
		sp<PipelineOutput> getOutput(PipelineOutputId id);
		
		void getOutputVector(Vector< sp<PipelineOutput> >& list);
	private:
		bool isValidId(PipelineOutputId id);
	protected:
		struct  OutputInfo
		{
			OutputInfo() {
				
			}
			sp<PipelineOutput> output;
		};

		Vector<OutputInfo> mOutputs;
		PipelineOutputId mNextOutputId;

		PipelineNode* mPipelineNode;
	};


}


#endif
