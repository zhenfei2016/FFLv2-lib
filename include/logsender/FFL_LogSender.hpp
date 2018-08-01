#ifndef _FFL_LOG_SENDER_HPP_
#define _FFL_LOG_SENDER_HPP_

#include <FFL.h>

namespace FFL {	
	class LogPipeline;
	class LogSender {
	public:
		LogSender();
		~LogSender();

		void startup();
		void shutdown();

		void write(int level,uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list args);
	private:
		LogPipeline* mLogInstance;
	};
}

#endif