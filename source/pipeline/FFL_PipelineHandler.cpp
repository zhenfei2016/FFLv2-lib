/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineHandler.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/11 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 * pipeline系统的消息处理句柄基类
 *
*/
#include <pipeline/FFL_PipelineHandler.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>

namespace FFL {

	PipelineHandler::PipelineHandler() {

	}
	PipelineHandler::~PipelineHandler() {

	}

	void PipelineHandler::onMessageReceived(const sp<Message> &msg) {
		if (msg.isEmpty()) {
			handleQuitMessage(0);
			return;
		}

		if (!isPipelineMessage(msg)) {
			FFL_LOG_WARNING("PipelineHandler::onMessageReceived not pipline msg");
			return;
		}

		sp<PipelineMessage> message = (PipelineMessage*)(msg->mObj.get());		
		if (isSysPipelineMessage(message)) {
			handleSysMessage(message);
		}else {
			handleMessage(message);
		}
	}

	//
	//  收到系统消息
	//
	void PipelineHandler::handleSysMessage(const sp<PipelineMessage> &msg)
	{
		FFL_LOG_INFO("handleSysMessage %d ",msg->getType());

	}

	//
	//  退出消息循环
	//
	void PipelineHandler::handleQuitMessage(uint32_t exitCode) {

	}
}
