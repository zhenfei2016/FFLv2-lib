#include "LogUploader.hpp"
#include "LogMessages.hpp"
#include "LogMessageId.hpp"

namespace FFL {

	LogUploader::LogUploader() {
		setName("LogUploader");
		mCreateWriterTime = 0;
		mWriter = NULL;
	}

	LogUploader:: ~LogUploader() {
	}
	//
	//  �����next��
	//
	void LogUploader::outputToCreator(NodeBase* next, const char* name, void* userdata) {
		mOutputWriter = connectNext(next, name, userdata);
	}
	void LogUploader::onCreate() {

	}
	FFL::sp<FFL::PipelineConnector > LogUploader::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input,
		void* userdata) {
		FFL::PipelineAsyncConnector* conn;
		if (mLooper == NULL) {
			conn= new FFL::PipelineAsyncConnector();
			mLooper=conn->getLooper();
		}
		else {
			conn= new FFL::PipelineAsyncConnector(mLooper);
		}
		return conn;
	}
	bool LogUploader::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		switch (msg->getType())
		{
		case LOG_MESSAGE_UPDATE_WRITER:
			handleUpdateWriter(msg, userdata);
			msg->consume(this);
			break;
		case LOG_MESSAGE_READER:
			handleLogInfo(msg,userdata);
			msg->consume(this);
			break;
		default:
			msg->consume(this);
			break;
		}		
		return true;
	}
	void LogUploader::handleLogInfo(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {		
		if (!mWriter.isEmpty()) {
			IOWriter* writer=mWriter->getWriter();			
			if (writer) {
				size_t writed = 0;
				LogInfo* info = (LogInfo*)(msg->getPayload());
				if (FFL_OK == writer->write((void*)(info->mInfo.c_str()),
					info->mInfo.size(), &writed)) {
					return;
				}
			}
		}
		tryCreateWriter();
	}

	//
	//  ����writer
	//
	void LogUploader::handleUpdateWriter(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		LogWriterMessage* payload=msg->getPayloadT<LogWriterMessage>();
		mWriter = payload->mWriter;
	}


	void LogUploader::tryCreateWriter() {
		if (mCreateWriterTime == 0 ||
			FFL_getNowUs() - mCreateWriterTime > (1000 * 1000) * 60) {
			//
			//  �Ƿ����һ�δ���һ������
			//
			mCreateWriterTime = FFL_getNowUs();
			FFL::PipelineMessage* controlMsg = new FFL::PipelineMessage(LOG_MESSAGE_CREATE_WRITER);
			postMessage(mOutputWriter.getId(), controlMsg);			
		}
	}

}


