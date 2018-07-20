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
*  pipeline中的事件处理回调
*
*/
#ifndef __FFL_PIPELINE_EVENT_CALLBACK_HPP_
#define __FFL_PIPELINE_EVENT_CALLBACK_HPP_

#include <ref/FFL_Ref.hpp>
namespace FFL{

	class PipelineEvent;

	//
	// PipelineEvent的回调函数  
	//
	class EventCallback : public RefBase {
	public:
		virtual void fire(const sp<PipelineEvent>& event) = 0;
	};
	
	//
	//  函数类型（）的回调函数
	//  void method(void);
	//
	class FunctionCallback : public  EventCallback {
	public:
		inline FunctionCallback( void(*method)(const sp<PipelineEvent>& event) ) :mMethod(method) {
		}

		void fire(const sp<PipelineEvent>& event) {
			mMethod(event);
		}
	private:
		void(*mMethod)(const sp<PipelineEvent>& event);
	};


	//
	//类成员函数类型的回调
	//void ClassName::Method();
	//
	template <typename ClassName>
	class ClassMethodCallback : public EventCallback
	{
	private:
		typedef void(ClassName::*Method)(const sp<PipelineEvent>& event);
	public:
		inline ClassMethodCallback(ClassName* instance,Method method)
			: mInstance(instance), mMethod(method)
		{}

		void fire(const sp<PipelineEvent>& event) {
			(mInstance->*mMethod)(event);
		}
	private:
		ClassName* mInstance;
		Method mMethod;
	};
}


#endif
