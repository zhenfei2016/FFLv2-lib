#ifndef _FFL_PIPELINE_DEFINED_HPP_
#define _FFL_PIPELINE_DEFINED_HPP_
namespace FFL{
	//
	//  节点id, 输入，输出接口id
	//
	typedef int PipelineNodeId;
	typedef int PipelineInputId;
	typedef int PipelineOutputId;

	//
	//  无效的id
	//
	enum {
		Pipeline_INVALID_Id = 0,
	};
}


#endif

