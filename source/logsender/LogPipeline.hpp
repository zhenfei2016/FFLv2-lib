/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogPipeline.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/08/05
*  https://github.com/zhenfei2016/FFL-v2.git
*  ��־������ˮ�ߣ�����������������
*
*/
#include <pipeline/FFL_NodeBase.hpp>
#include <logsender/FFL_LogSenderType.hpp>
#include "LogCollector.hpp"

namespace FFL {
	class LogUploader;
	class LogPipeline  {
	public:
		LogPipeline();
		~LogPipeline();	
		//		
		//  type��Ŀ����־������
		//  url : Ŀ����־��·��	
		//  ����Ŀ���ļ���
		//
		void setTargetUrl(LogSenderType type, const char* url);
		void getTargetUrl(LogSenderType& type, String& url);
		//
		//  ������ֹͣLogPipeline
		//
		bool startup();
		void shutdown();        
		//
		//  д��־��Ϣ
		//
		void write(int level, uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list v);
	private:
		bool isStartup();
	private:
		FFL::NodeManager* mManager;
		FFL::sp<FFL::Pipeline> mPipeline;
		//
		// �ռ���
		//
		FFL::sp<LogCollector> mCollector;
		FFL::sp<LogUploader> mUploader;
		
		//
		//  ����������
		//
		LogSenderType mType;
		String mUrl;
	};
}


