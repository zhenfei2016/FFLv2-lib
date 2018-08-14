/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogWriterCreator.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/08/06
*  https://github.com/zhenfei2016/FFL-v2.git
*  创建日志writer
*
*/
#ifndef _FFL_LOG_WRITER_CREATOR_HPP_
#define _FFL_LOG_WRITER_CREATOR_HPP_


#include <pipeline/FFL_NodeBase.hpp>
#include <utils/FFL_Io.hpp>
#include <logsender/FFL_LogSenderType.hpp>
#include "LogMessages.hpp"

namespace FFL {

	class LogWriterCreator :public FFL::NodeBase {
	public:
		LogWriterCreator(LogSenderType type,const char* url);
		virtual ~LogWriterCreator();

		//
		//  连接当前节点的创建的writer ， output到next中
		//
		void connectOutputWriterTarger(NodeBase* next, const char* name, void* userdata);
	private:
		//
		//  发送新创建的weiter到target中
		//
		void postNewWriterToTager(FFL::sp<RefCountWriter> writer);
		//*
		//  创建网络类型的writer
		//
		FFL::sp<RefCountWriter> createNetWriter();
		//
		//  创建文件类型的writer
		//
		FFL::sp<RefCountWriter> createFileWriter();
	protected:
		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
	private:
	private:		
		OutputInterface mOutputWriter;	

		//
		//  创建的类型
		//
		LogSenderType mType;
		String mUrl;
	};
}

#endif