/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineOutput.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  node的输出接口
*
*/
#ifndef _FFL_PIPELINE_OUTPUT_HPP_
#define _FFL_PIPELINE_OUTPUT_HPP_
#include <ref/FFL_Ref.hpp>

#include <pipeline/FFL_PipelineDefined.hpp>
#include <utils/FFL_Flags.hpp>
#include <thread/FFL_Mutex.hpp>

namespace FFL
{
	class PipelineNode;
	class PipelineMessage;
	class PipelineConnector;

	class PipelineOutput : public RefBase{
	public:				
		friend class PipelineNode;
		friend class PipelineOutputList;
	protected:
		PipelineOutput(PipelineNode* node, PipelineOutputId id);
		~PipelineOutput();
		void setId(PipelineOutputId id) { mId = id; }
	public:
		PipelineOutputId getId() const;
		//
		//  连接输出端到这个上面		
		//
		void connect(sp<PipelineConnector> conn);
		//
		// 断掉这个连接
		//
		void disconnect();			
		//
		// post消息 , 
		// 如果这个输出接口没有连接到一个有效的PipelineConnector上则发送失败
		// msg :消息值
		//
		status_t postMessage(const sp<PipelineMessage> &msg, uint64_t delayUs);
		//
		//  清空等待处理的消息
		//
		void clearMessage();
		//
		// 获取待处理的消息的大小
		//
		int64_t getPendingMessageSize();
	protected:
		PipelineOutputId mId;
	private:
		CMutex mLock;
		PipelineNode* mPipelineNode;
		sp<PipelineConnector> mConnector;

		//
		// 状态标志
		//
		enum {
			CONNECTED = 0x01,			
		};
		Flags32b mFlags;
	};
}
#endif
