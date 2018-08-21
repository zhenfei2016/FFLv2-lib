/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogCollector.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/07 
*  https://github.com/zhenfei2016/FFL-v2.git
*  �ռ���־
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
		// ������־�ռ���
		//
		void reset();
		//
		//  ��reader���������
		//
		void collect(FFL::String& info);
		//
		//  �����next��
		//
		void connectOutputToUploader(NodeBase* next, const char* name, void* userdata);
	private:
		//
		//  �ɹ�������node
		//
		virtual void onCreate();

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

	private:
		FFL::CMutex mInfoLock;
		FFL::List< FFL::String> mInfoList;
		FFL::sp<FFL::PipelineMessageCache > mMessageCache;
		OutputInterface mOutputUploader;

		//
		//  ��־̫���ˣ������˶�����
		//
		int32_t mDiscardNum;
	};
		
}


#endif