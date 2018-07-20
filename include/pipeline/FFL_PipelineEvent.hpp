/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineEvent.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/05/01
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  pipeline中的事件处理系统
*
*/
#ifndef _FFL_PIPELINEEVENT_HPP_
#define _FFL_PIPELINEEVENT_HPP_


#include <utils/FFL_Message.hpp>
#include <pipeline/FFL_PipelineDefined.hpp>

namespace FFL{

	class Pipeline;
	class PipelineEvent;	
	class PipelineEventLooper;
	class EventCallback;

	class PipelineEvent : public Message { 
		enum {
			PipelineEventWhat = 1234,
		};
	public:
		PipelineEvent(int32_t eventId);
		PipelineEvent(PipelineNodeId src, PipelineNodeId dst,int32_t eventId);
		PipelineEvent(sp<EventCallback> callback);

		void setEventId(int32_t eventId);
		int32_t getEventId() const;
		//
		// 设置这个消息带的参数，
		//
		void setParams(int64_t param1, int64_t param2);
		int64_t getParam1() const;
		int64_t getParam2() const;
	public:
		//
		//是否回调方式的
		//
		bool haveCallback();
	
		friend class PipelineEventHandler;

		//
		//执行回调
		//
		void fireCallback();
	protected:
		//
		//事件的id
		//
		uint32_t mEventId;

		//
		//这个消息的源发送节点id
		//
		PipelineNodeId mSrcNodeId;
		//
		//这个消息的目标处理节点id
		//如果节点为 INVALID_NodeId ,则所有节点进行处理
		//
		PipelineNodeId mTargetNodeId;

		//
		// callback
		//
		sp<EventCallback> mCallback;

		//
		//  参数
		//
		int64_t mParam1;
		int64_t mParam2;
	};
	
	//
	//  消息处理的looper
	//	
	class PipelineEventLooper : public Looper {
	public:
		PipelineEventLooper();
		PipelineEventLooper(sp<Pipeline> pipeline);
		
		//
		//  post一条事件到这个looper中
		//
		void postEvent(const sp<PipelineEvent> &event, int64_t delayUs);
	protected:
		//
		// 收到一个事件
		//
		void onEvent(const sp<PipelineEvent> &event);

    private:
		//
		//  事件的处理句柄
		//
		friend class EventHandler;
		sp<Handler> mEventHandler;
		wp<Pipeline> mPipeline;
		bool mHavePipeline;
	};
}


#endif
