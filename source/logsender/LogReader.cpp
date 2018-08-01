
#include "LogReader.hpp"
#include "LogMessageId.hpp"
#include "LogMessages.hpp"

namespace FFL {
	LogReader::LogReader() {
		setName("logReader");
		mMessageCache = new FFL::PipelineMessageCache(LOG_MESSAGE_READER);
	}

	LogReader::~LogReader() {
	}
	//
	//  向reader中填充数据
	//
	void LogReader::fillData(FFL::String& info) {
		FFL::CMutex::Autolock l(mInfoLock);
		mInfoList.push_back(info);
	}
	//
	//  输出到next中
	//
	void LogReader::outputTo(NodeBase* next, const char* name, void* userdata) {
		mOutputUploader = connectNext(next, name, userdata);
	}
	//
	//  成功创建了node
	//
	void LogReader::onCreate() {	
		connectTimerInput("logReader", 1000, 0);
	}
	//
	//  获取数据循环函数
	//
	bool LogReader::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		FFL::String infoString;
		{
			FFL::CMutex::Autolock l(mInfoLock);
			while (mInfoList.size() > 0) {
				infoString += mInfoList.front() + "\r";
				mInfoList.pop_front();
			}
		}

		if (infoString.size()) {
			LogInfo* info = NULL;
			FFL::sp<FFL::PipelineMessage> msg = mMessageCache->createMessage(&info, LOG_MESSAGE_READER);
			info->mInfo = infoString;
			if (FFL_OK != postMessage(mOutputUploader.getId(), msg)) {
				msg->consume(this);
			}
		}
		return true;
	}
}


