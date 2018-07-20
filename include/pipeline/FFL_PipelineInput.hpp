/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineInput.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/10
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  node的输入接口
*
*/
#ifndef _FFL_PIPELINE_INPUT_HPP_
#define _FFL_PIPELINE_INPUT_HPP_

#include <ref/FFL_Ref.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>
#include <utils/FFL_Flags.hpp>
#include <utils/FFL_String.hpp>
#include <pipeline/FFL_PipelineDefined.hpp>
#include <pipeline/FFL_PipelineInputHandler.hpp>

namespace FFL{
	class PipelineConnector;
	class PipelineNode;
	class PipelineMessage;
	class PipelineLooper;

	class Handler;
	class PipelineInputHandler;
	
	class PipelineInput : public RefBase{
		friend class PipelineInputList;
		friend class PipelineNode;
	protected:
		//
		// pipelineNode : input所属的Node
		// handler :处理输入的消息句柄
		//
		PipelineInput(PipelineNode* pipelineNode, sp<PipelineInputHandler> handler, PipelineInputId id);
		~PipelineInput();

		void setId(PipelineInputId id);
		void setName(const char* name);
	public:
		//
		//  获取这个Input在PipelineNode中的唯一id,
		//
		PipelineInputId getId() const;
		void getName(String& name) const;
		//
		//  连接输出端到这个上面
		//  conn   ：output与这个input是如何连接的
		//  output ：这是数据的输入节点
		//
		void connect(sp<PipelineConnector> conn);
		//
		// 断掉这个连接 ,只有当前不是在startup 也不在shutdown的时候才可以disconnect
		//
		void disconnect();	
		//
		// 启动停止 输入
		// 同步启动停止，如果存在线程的话
		// 输入线程已经运行起来了 。 或者输入线程已经结束了
		//
		status_t startup();
		status_t shutdown();
		// 
		// 异步请求shutdown
		//
		status_t requestShutdown();
		status_t waitShutdown();

		bool isStarted() ;
	private:
		//
		//  发送成功启动的消息
		//
		void postStartSuccessMessage();
	public:
		//
		//  发送消息给自己
		//
		status_t postSelfMessage(const sp<PipelineMessage>& msg,int64_t delayUs);
		//
		//  清空等待处理的消息
		//
		void clearMessage();
		//
        // 分派系统消息，马上执行的， post的消息最终也会走这地方的
        //
        void dispathSysMessage(const sp<PipelineMessage>& msg);
		//
		// 分派消息，马上执行的， post的消息最终也会走这地方的
		//
		void dispathMessage(const sp<PipelineMessage>& msg);					
	protected:	
		//
		//  输入接口的名称
		//
		String mName;

		CMutex mLock;
		friend class PipelineNode;
		PipelineInputId mId;
		//
		// 状态标志
		//
		enum {
			CONNECTED=0x01,
			STARTING=0x02,
			LOOPING= 0x04,
			SHUTDOWNING= 0x08,
		};
		Flags32b mFlags;
		//
		// 所有者
		//
		PipelineNode* mPipelineNode;
		//
		// 连接状态，连接器
		//
		bool mConnected;
		sp<PipelineConnector> mConnector;

		//
		//  外部设置的处理句柄
		//
		sp<PipelineInputHandler> mInputHandler;
	};	
}


#endif
