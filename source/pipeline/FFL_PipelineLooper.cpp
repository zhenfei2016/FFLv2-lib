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

#include <pipeline/FFL_PipelineLooper.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>

namespace FFL {
	PipelineLooper::PipelineLooper(sp<Clock> clock):Looper(clock) {
		
	}
	//
	//  发送消息
	//
	bool PipelineLooper::postMessage(const sp<PipelineMessage> &msg, Looper::handler_id target, int64_t delayUs) {
		sp<Message> message = new Message();
		message->setWhat(MSGT_PIPELINE);
		message->setTarget(target);
		message->mObj = msg;
		msg->mMessageUniqueId = message->uniqueId();
		post(message, delayUs);
		return true;
	}
	//
	// 发送退出looper消息
	//
	void PipelineLooper::postQuitMessage()
	{
		FFL_ASSERT_LOG(0, "PipelineLooper::postQuitMessage");
		//Looper::postQuitMessage(1);
	}
	//
	// 清空等待执行的queue 消息
	//
	void PipelineLooper::flushMessage() {
		FFL_ASSERT_LOG(0, "PipelineLooper::flushMessage");
        //List< sp<Message> > list;
        //getMessageList(list);
	}
	//
	//  清空一条消息
	//
	bool PipelineLooper::clearMessage(const sp<PipelineMessage> &msg) {
		if (msg.isEmpty()) {
			return false;
		}

		if (cancelMessage(msg->mMessageUniqueId)) {		
			msg->consume(this);
			return true;
		}
		return false;
	}
	//
	//  清空特定handler消息
	//
	void PipelineLooper::clearMessage(Looper::handler_id id) {
		List< sp<Message> > list;
		clearMessageList(&list,id);

		FFL_LOG_DEBUG("PipelineLooper:clearMessage count=%d", list.size());
		for (List< sp<Message> >::iterator it = list.begin(); it != list.end(); it++) {
			sp<Message> msg = *it;
			if (msg.isEmpty()) {
				continue;
			}

			sp<PipelineMessage> message = getPipelineMessage(msg);
			if (!message.isEmpty()) {
				message->consume(this);
			}
		}
	}

	void PipelineLooper::clearMessage()
	{
		List< sp<Message> > list;
		clearMessageList(&list);

		for (List< sp<Message> >::iterator it = list.begin(); it != list.end(); it++) {
			sp<Message> msg = *it;
			if (msg.isEmpty()) {
				continue;
			}

			sp<PipelineMessage> message = getPipelineMessage(msg);
			if (!message.isEmpty()) {
				message->consume(this);
			}
		}
	}
}
