/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogUploader.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*
*
*/
#ifndef _FFL_LOG_UPLOADER_HPP_
#define _FFL_LOG_UPLOADER_HPP_

#include <pipeline/FFL_NodeBase.hpp>
#include <net/FFL_NetSocket.hpp>
#include <logsender/FFL_LogSenderType.hpp>


namespace FFL {
	class RefCountWriter;
	class LogUploader :public FFL::NodeBase {
	public:
		LogUploader();
		virtual ~LogUploader();

		//
		//  ˢ��һ���µ�writer�����������´���
		//
		void refrushWriter(LogSenderType type, const String& url);
		//
		//  �����next��,����һ��creator
		//
		void connectOutputToCreator(NodeBase* next, const char* name, void* userdata);
	protected:	
		FFL::sp<FFL::PipelineConnector > onCreateConnector(	const OutputInterface& output,
			const InputInterface& input,
			void* userdata) ;

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
		//
		//  ������־��Ϣ
		//
		void handleLogInfo(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
		//
		//  ����writer
		//
		void handleUpdateWriter(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
	private:
		//
		//  ����writer
		//
		void tryCreateWriter();
		void createWriter();
	private:		
		OutputInterface mOutputWriter;
		//
		//  �ϴδ���writer��ʱ��
		//
		int64_t mCreateWriterTime;
		FFL::sp<FFL::RefCountWriter> mWriter;

		FFL::sp<FFL::PipelineLooper > mLooper;

		//
		//  ����Ŀ��
		//
		LogSenderType mType;
		String mUrl;
	};
}

#endif




