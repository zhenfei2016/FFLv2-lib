#ifndef _FFL_LOG_SENDER_HPP_
#define _FFL_LOG_SENDER_HPP_

#include <helper/FFL_NodeBase.hpp>
namespace FFL {	
	class LogPipeline;
	class LogSender {
	public:
		LogSender();
		~LogSender();

		void startup();
		void shutdown();

		void write(uint8_t* data, int32_t len);
	private:
		LogPipeline* mLogInstance;
	};
}

#endif