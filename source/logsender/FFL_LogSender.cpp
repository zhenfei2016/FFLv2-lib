#include <logsender/FFL_LogSender.hpp>
#include <net/base/FFL_Net.h>
#include "LogPipeline.hpp"

namespace FFL {
	LogSender::LogSender() {
		FFL_socketInit();
		mLogInstance = new LogPipeline();
	}
	LogSender::~LogSender() {
		mLogInstance->shutdown();
		FFL_SafeFree(mLogInstance);
	}

	void LogSender::startup() {
		mLogInstance->startup();
	}
	void LogSender::shutdown() {
		mLogInstance->shutdown();
	}

	void LogSender::write(int level, uint8_t* data, int32_t len) {
		mLogInstance->write(level, data, len);
	}
	void LogSender::write(int level, const char* tag, const char *format, va_list args) {
		mLogInstance->write(level, tag,format,args);
	}



}

static int printLogToSender(int level, const char* tag, const char *format, va_list args, void* userdata) {
	FFL::LogSender* sender = (FFL::LogSender*)userdata;
	if (sender) {
		sender->write(level, tag, format, args);
	}
	return 1;
}
//
//  接管log系统
//
extern "C"  void FFL_hookLogSystem(FFL::LogSender& sender) {
	FFL_LogHook(printLogToSender, &sender);
}
extern "C"  void FFL_unhookLogSystem() {
	FFL_LogHook(0, 0);
}