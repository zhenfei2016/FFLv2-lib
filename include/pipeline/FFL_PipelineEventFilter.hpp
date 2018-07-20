/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineEvent.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/05/01
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  pipeline中的事件Filter,可截获，中断事件处理
*
*/
#ifndef _FFL_PIPELINE_EVENTFILTER_HPP_
#define _FFL_PIPELINE_EVENTFILTER_HPP_

#include <ref/FFL_Ref.hpp>
namespace FFL{
	class PipelineEvent;
	class PipelineEventFilter{
	public:
	    //
		// 派发消息前的准备，消息派发前会先到这地方的
		// 返回false表示不要向下派发了
		//
		virtual bool onPrepareDispatch(const sp<PipelineEvent> event) = 0;
	};
}


#endif
