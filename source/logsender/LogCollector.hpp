/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogCollector.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/07 
*  https://github.com/zhenfei2016/FFL-v2.git
*  收集日志
*
*/
#ifndef _FFL_LOG_COLLECTOR_HPP_
#define _FFL_LOG_COLLECTOR_HPP_


#include <pipeline/FFL_NodeBase.hpp>

namespace FFL {
	class LogCollector : public NodeBase {
	public:
		LogCollector();
		virtual ~LogCollector();

		//
		// 重置日志收集器
		//
		void reset();
		//
		//  向reader中填充数据
		//
		void collect(FFL::String& info);
		//
		//  输出到next中
		//
		void connectOutputToUploader(NodeBase* next, const char* name, void* userdata);
	private:
		//
		//  成功创建了node
		//
		virtual void onCreate();

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

	private:
		FFL::CMutex mInfoLock;
		FFL::List< FFL::String> mInfoList;
		FFL::sp<FFL::PipelineMessageCache > mMessageCache;
		OutputInterface mOutputUploader;

		//
		//  日志太多了，丢弃了多少条
		//
		int32_t mDiscardNum;
	};
		
}


#endif