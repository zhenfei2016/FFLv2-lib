#include <logsender/FFL_LogSender.hpp>
#include "LogPipeline.hpp"

namespace FFL {
	LogSender::LogSender() {
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