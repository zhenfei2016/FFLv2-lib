/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineEvent.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/05/01
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  pipeline中的事件处理系统
 *
*/
#include <pipeline/FFL_Pipeline.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineEventCallback.hpp>
#include <utils/FFL_Handler.hpp>
#include <utils/FFL_Looper.hpp>


namespace FFL {
	PipelineEvent::PipelineEvent(int32_t eventId) {
		mEventId = eventId;
		setWhat(PipelineEventWhat);
		mParam1 = 0;
		mParam2 = 0;
	}
	PipelineEvent::PipelineEvent(PipelineNodeId src, PipelineNodeId dst, int32_t eventId) {
		mSrcNodeId = src;
		mTargetNodeId = dst;
		mEventId = eventId;
		mParam1 = 0;
		mParam2 = 0;
	}

	PipelineEvent::PipelineEvent(sp<EventCallback> callback) :mCallback(callback) {
	}
	void PipelineEvent::setEventId(int32_t eventId) {
		mEventId = eventId;
	}
	int32_t PipelineEvent::getEventId() const {
		return mEventId;
	}

	bool PipelineEvent::haveCallback() {
		return !(mCallback.isEmpty());
	}
	//执行回调
	void PipelineEvent::fireCallback() {
		if (!mCallback.isEmpty())
			mCallback->fire(this);
	}
	//
	// 设置这个消息带的参数，
	//
	void PipelineEvent::setParams(int64_t param1, int64_t param2) {
		mParam1 = param1;
		mParam2 = param2;
	}
	int64_t PipelineEvent::getParam1() const {
		return mParam1;
	}
	int64_t PipelineEvent::getParam2() const {
		return mParam2;
	}
	//
	//
	//
	class EventHandler : public Handler {
	public:
		EventHandler(PipelineEventLooper* looper) :mLooper(looper) {
		}
	private:
		void onMessageReceived(const sp<Message> &msg) {
			if (msg.isEmpty()) {
				return;
			}
			//
			//  如果这个事件有自己的处理回调，则执行
			//
			sp<PipelineEvent> event = ((PipelineEvent*)msg.get());
			if (event->haveCallback()) {
				event->fireCallback();
			}
			else {
				mLooper->onEvent(event);
			}
		}
		PipelineEventLooper* mLooper;
	};
	PipelineEventLooper::PipelineEventLooper():mHavePipeline(false){
        mEventHandler = new EventHandler(this);
	}
	PipelineEventLooper::PipelineEventLooper(sp<Pipeline> pipeline): mHavePipeline(true)
	{
		mPipeline = pipeline;
        mEventHandler = new EventHandler(this);
	}
	void PipelineEventLooper::postEvent(const sp<PipelineEvent> &event, int64_t delayUs)
	{
		sp<Pipeline> pipeline;
		Looper::handler_id id = mEventHandler->id();
		if (mHavePipeline) {
			if (id == 0) {
				pipeline = mPipeline.promote();
				if (!pipeline.isEmpty()) {
					id = registerHandler(mEventHandler);
				}
			}
			else {
				pipeline = mPipeline.promote();
			}

			if (!pipeline.isEmpty()) {
				event->setTarget(id);
				post(event, delayUs);
			}
		}
		else {
			Looper::handler_id id = mEventHandler->id();
			if (id == 0) {
				id = registerHandler(mEventHandler);
			}

			event->setTarget(id);
			post(event, delayUs);
		}		
	}
	void PipelineEventLooper::onEvent(const sp<PipelineEvent> &event){		
		sp<Pipeline> pipeline = mPipeline.promote();
		if (!pipeline.isEmpty()) {
			pipeline->onEvent(event);
		}
	}
}
