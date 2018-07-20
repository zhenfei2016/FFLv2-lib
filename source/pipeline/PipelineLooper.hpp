/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineLooper.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/16
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  pipeline消息系统的looper,处理pipelineMessage
*
*/

#ifndef __FFL_PIPELINE_MESSAGE_LOOPER_HPP_
#define __FFL_PIPELINE_MESSAGE_LOOPER_HPP_

#include <utils/FFL_Looper.hpp>

namespace FFL{	
	class PipelineMessage;
	class PipelineLooper : public Looper { 		
	public:
		PipelineLooper(sp<Clock> clock);

		//
		// 发送一个消息
		//
		bool postMessage(const sp<PipelineMessage> &msg, Looper::handler_id target,int64_t delayUs=0);
		//
		// 发送退出looper消息
		//
		void postQuitMessage();
		//
		// 清空等待执行的消息
		//
		void flushMessage();
		//
		//  清空一条消息
		//
		bool clearMessage(const sp<PipelineMessage> &msg);
		//
		//  清空特定handler的所有消息
		//
		void clearMessage(Looper::handler_id id);
		//
		//  清空所有消息
		//
		void clearMessage();
	};
}


#endif
