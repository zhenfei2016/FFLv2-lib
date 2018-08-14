/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  LogWriterCreator.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/08/06 
*  https://github.com/zhenfei2016/FFL-v2.git
*  ������־writer
*
*/
#include "LogWriterCreator.hpp"
#include "LogMessageId.hpp"
#include "LogMessages.hpp"
#include <net/base/FFL_Net.h>
#include <net/FFL_NetSocket.hpp>
#include <utils/FFL_File.hpp>
#include <net/FFL_NetUtils.hpp>

namespace FFL {

	class NetWriter : public RefCountWriter {
	public:
		NetWriter(NetFD fd){
			mSocket = new CSocket(fd);
		}
		virtual ~NetWriter() {
			if (mSocket != NULL) {
				FFL_SafeFree(mSocket);
			}
		}
		virtual IOWriter* getWriter() {
			return mSocket;
		}
		CSocket* mSocket;
	};


	class FileWriter : public RefCountWriter {
	public:
		FileWriter(const char* path) {
			mFile = new File();		
		}


		virtual ~FileWriter() {
			if (mFile != NULL) {
				FFL_SafeFree(mFile);
			}
		}
		virtual IOWriter* getWriter() {
			return mFile;
		}
		File* mFile;
	};


	LogWriterCreator::LogWriterCreator(LogSenderType type, const char* url):mType(type) {
		mUrl = url?url:"";
		setName("LogWriterCreator");		
	}

	LogWriterCreator:: ~LogWriterCreator() {
	}

	void LogWriterCreator::connectOutputWriterTarger(NodeBase* next, const char* name, void* userdata) {
		mOutputWriter = connectNext(next, name, userdata);
	}

	//
	//  �����´�����weiter��target��
	//
	void LogWriterCreator::postNewWriterToTager(FFL::sp<RefCountWriter> writer) {
		FFL::PipelineMessage* controlMsg = new FFL::PipelineMessage(LOG_MESSAGE_UPDATE_WRITER);
		LogWriterMessage* msg = new LogWriterMessage();
		msg->mWriter = writer;
		controlMsg->setPayload(msg);
		postMessage(mOutputWriter.getId(), controlMsg);
	}

	//
	//  �յ���Ϣ
	//
	bool LogWriterCreator::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		if (msg->getType() == LOG_MESSAGE_CREATE_WRITER) {
			//
			//  ����һ���µ�writer
			//
			FFL::sp<RefCountWriter> writer;
			switch (mType)
			{
			case FFL::LOG_ST_TCP_CONNECT:
				writer = createNetWriter();
				break;
			case FFL::LOG_ST_EXIST_FILE:
			{
				FileWriter* fileWriter = new FileWriter(mUrl.c_str());
				fileWriter->mFile->open(mUrl);
				writer = fileWriter;
			}
				break;
			case FFL::LOG_ST_NEW_FILE:
			{
				FileWriter* fileWriter = new FileWriter(mUrl.c_str());
				fileWriter->mFile->create(mUrl);
				writer = fileWriter;
			}
				break;
			default:
				break;
			}			
			postNewWriterToTager(writer);
		}
		msg->consume(this);
		return true;
	}
	//
	//  �����������͵�writer
	//
	FFL::sp<RefCountWriter> LogWriterCreator::createNetWriter() {
		NetFD fd = 0;

		String ip;
		int32_t port;
		FFL_parseHostport(mUrl, ip, port);
		if (port > 0 && FFL_isIp(ip) == FFL_OK) {
		   FFL_socketNetworkTcpClient(ip.c_str(), port, &fd);
	    }

		if (fd > 0) {
			return new NetWriter(fd);;
		}
		return NULL;
	}
	//
	//  �����ļ����͵�writer
	//
	FFL::sp<RefCountWriter> LogWriterCreator::createFileWriter() {
		FileWriter*writer= new FileWriter(mUrl.c_str());
		return writer;
	}
}


