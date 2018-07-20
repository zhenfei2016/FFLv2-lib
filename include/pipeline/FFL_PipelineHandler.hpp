/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineHandler.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
* pipeline系统的消息处理句柄基类
*
*/

#ifndef _FFL_PIPELINE_HANDLER_HPP_
#define _FFL_PIPELINE_HANDLER_HPP_

#include <utils/FFL_Handler.hpp>

namespace FFL
{	
	class PipelineMessage;	
	
	//
	//异步连接器   
    //
	class PipelineHandler : public Handler {
	public:
		PipelineHandler();
		virtual  ~PipelineHandler();

	protected:
		void onMessageReceived(const sp<Message> &msg) ;

	protected:
		//
		//  收到普通用户消息
		//
		virtual void handleMessage(const sp<PipelineMessage> &msg)=0;
		//
		//  收到系统消息
		//
		virtual void handleSysMessage(const sp<PipelineMessage> &msg);
		//
		//  退出消息循环
		//
		virtual void handleQuitMessage(uint32_t exitCode);
		

	};	
}


#endif
