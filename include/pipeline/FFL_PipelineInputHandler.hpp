/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineInputList.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/16
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  输入接口回调函数，外部主要需要继承这个
*
*/

#ifndef _FFL_PIPELINE_INPUT_HANDLER_HPP_
#define _FFL_PIPELINE_INPUT_HANDLER_HPP_

#include <ref/FFL_Ref.hpp>
namespace FFL{	
	
	class PipelineMessage;

	//
	//  PipelineInput 输入消息的处理接口
	//
	class PipelineInputHandler:public RefBase {
	public:
		virtual ~PipelineInputHandler() {}
		virtual void handleMessage( const sp<PipelineMessage>& msg)=0;
	};

	//
	//类成员函数类型的处理接口
	//void ClassName::Method(const sp<PipelineMessage>& msg);
	//
	template <typename ClassName>
	class ClassMethodPipelineInputHandler : public PipelineInputHandler
	{
	private:
		typedef void(ClassName::*Method)(const sp<PipelineMessage>& msg);
	public:
		inline ClassMethodPipelineInputHandler(ClassName* instance, Method method)
			: mInstance(instance), mMethod(method)
		{}

		void handleMessage(const sp<PipelineMessage>& msg) {
			(mInstance->*mMethod)(msg);
		}
	private:
		ClassName* mInstance;
		Method mMethod;
	};	

	//
	//类成员函数类型的处理接口
	//void ClassName::Method();
	//
	template <typename ClassName>
	class ClassMethodVoidInputHandler : public PipelineInputHandler
	{
	private:
		typedef void(ClassName::*Method)();
	public:
        inline ClassMethodVoidInputHandler(ClassName* instance, Method method):
                mInstance(instance),
                mMethod(method)
        {}
		void handleMessage(const sp<PipelineMessage>& msg) {
			(mInstance->*mMethod)();
		}
	private:
		ClassName* mInstance;
		Method mMethod;
	};

	
    template <typename ClassName,typename  ARG0 >
    class ClassMethodInputHandler : public PipelineInputHandler
    {
    private:
        typedef void(ClassName::*Method)(const sp<PipelineMessage>& msg, ARG0 a0);
    public:
        inline ClassMethodInputHandler(ClassName* instance, Method method,ARG0 arg):
        mInstance(instance),
        mA0(arg),
        mMethod(method)
        {}
        void handleMessage(const sp<PipelineMessage>& msg) {
            (mInstance->*mMethod)(msg, mA0);
        }
    private:
        ClassName* mInstance;
        ARG0 mA0;
        Method mMethod;
    };

}


#endif
