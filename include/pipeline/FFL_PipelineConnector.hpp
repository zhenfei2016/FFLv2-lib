/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineConnector.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  node连接器基类
*
*/


#ifndef _FFL_PIPELINE_CONNECTOR_HPP_
#define _FFL_PIPELINE_CONNECTOR_HPP_

#include <ref/FFL_Ref.hpp>
#include <thread/FFL_Mutex.hpp>
#include <utils/FFL_Looper.hpp>


namespace FFL
{
	class PipelineInput;
	class PipelineOutput;
	class PipelineMessage;	
	class PipelineLooper;

	/*
	 * 连接器
     * 数据流：
     *  PipelineOutput  --> PipelineConnector --> PipelineInput
     *
	 */
	class PipelineConnector : public RefBase{
		friend class PipelineInput;
		friend class PipelineOutput;
	public:
		PipelineConnector();
		virtual ~PipelineConnector();

		const char* getName() const;
		void setName(const char* name);

		//
		//  是否是连接状态， 有输入输出端
		//
		virtual bool isConnected();
	protected:
		//
		//  启动，停止这个连接器
		//
		virtual status_t startup();
		//
		//  请求并等待shutdown结束
		//
		virtual status_t shutdown();
		//
		//  请求shutdown,等待shutdown结束
		//
		virtual status_t requestShutdown();
		virtual status_t waitShutdown();
		//
		// 这个connector在哪一个looper中进行处理
		//
		virtual sp<PipelineLooper> getLooper();
		//
		// 连接器转发消息
		// delayUs:什么时候转发
		//
		virtual status_t tranport(const sp<PipelineMessage> &msg, int64_t delayUs)=0;
		//
		// 转发的消息已经处理了
		//
		virtual void consumer(const sp<PipelineMessage> &msg);
		//
		//  清空转发的消息
		//
		virtual void clearMessage()=0;
		//
		// 获取待处理的消息的大小
		//
		virtual int64_t getPendingMessageSize();
    protected:
        //
        // 开始分派系统消息
        //
        virtual void dispathSysMessage(const sp<PipelineMessage>& msg);
        //
        // 开始分派消息
        //
        virtual void dispathMessage(const sp<PipelineMessage>& msg){}
        
	protected:
        //
        //  设置这个连接器的输出节点
        //
		void setInput(PipelineInput* input);
        //
        //  设置这个连接器的输如节点
        //
		void setOutput(PipelineOutput* output);
		sp<PipelineInput> getInput();
		sp<PipelineOutput> getOutPut();
	private:
		CMutex mMutex;
		PipelineInput* mInput;
		PipelineOutput* mOutput;	

	private:
		const char* mName;
	};
		
	/*
	*  同步连接器
	*/
	class SyncPipelineConnector : public PipelineConnector {
	public:
		SyncPipelineConnector();
		//
		// 连接器转发消息
		// delayUs:同步连接的情况下这个参数无效的
		//
		status_t tranport(const sp<PipelineMessage> &msg, int64_t delayUs = 0);
		//
		//  清空转发的消息
		//
		void clearMessage() ;
		//
		// 获取待处理的消息的大小
		//
		int64_t getPendingMessageSize();
	};
}


#endif
