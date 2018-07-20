/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineAsyncConnector.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2017/12/10
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *
 *  node异步连接器,进行消息处理中消息的缓存异步处理
 *
*/

#ifndef _FFL_PIPELINE_ASYNCCONNECTOR_HPP_
#define _FFL_PIPELINE_ASYNCCONNECTOR_HPP_


#include <pipeline/FFL_PipelineConnector.hpp>
#include <thread/FFL_Mutex.hpp>
#include <utils/FFL_Flags.hpp>
#include <utils/FFL_Clock.hpp>


namespace FFL
{	
	class PipelineInput;
	class PipelineOutput;
	class PipelineMessage;	
	class PipelineLooper;	
	
	//
	//异步连接器   
    //
	class PipelineAsyncConnector : public PipelineConnector {
	public:
		//
		//looper : 这个异步连接器在哪一个looper中跑
		//
		PipelineAsyncConnector();
		PipelineAsyncConnector(sp<PipelineLooper> looper);
		virtual  ~PipelineAsyncConnector();
		//
		//  启动，停止这个连接器
		//
		status_t startup();
		status_t shutdown();
		//
		//  请求shutdown,等待shutdown结束
		//
		status_t requestShutdown();
		status_t waitShutdown();
		//
		// 这个connector在哪一个looper中进行处理
		//
		sp<PipelineLooper> getLooper();	
		//
		// 连接器转发消息
		//
		status_t tranport(const sp<PipelineMessage> &msg ,int64_t delayUs );
		//
		//  清空转发的消息
		//
		void clearMessage();
		//
		//  获取时钟，这个是tranport使用的，delayUs会通过这个时钟进行计算延迟多长时间
		//
		FFL::sp<FFL::Clock> getClock();
	public:
		void setDebug(bool debug);
	protected:
		//
		// 消息异步发送
		//
		status_t postMessage(const sp<PipelineMessage> &msg,int64_t delayUs=0);
        //
		// 开始分派消息
		//
		virtual void dispathMessage(const sp<PipelineMessage>& msg);
	private:
		//
		//  注册，反注册处理句柄
		//
		void registerHandler();
		void unregisterHandler();
	private:
		//
		//  connector所属的looper
		//
		sp<PipelineLooper>  mLooper;
		bool mIsOuterLooper;

		sp<Clock> mLooperClock;
		//
		//  消息处理句柄
		//
		class InputHandler;
		sp<InputHandler> mHandler;
	private:
		//
		// 状态标志
		//
		enum {	
			READYING = 0x0,
			STARTING = 0x01,			
			SHUTDOWNING = 0x02,
		};
		mutable Flags32b mFlags;
	public:
		//
		//  是否已经停止运行了
		//
		bool isShutdowned() const;
	};	
}


#endif
