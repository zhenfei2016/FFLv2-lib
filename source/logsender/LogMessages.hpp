#ifndef _LOG_MESSAGE_HPP_
#define _LOG_MESSAGE_HPP_

#include <pipeline/FFL_PipelineMessage.hpp>
#include <utils/FFL_Io.hpp>

namespace FFL {
	//
	//  日志信息
	//
	class LogInfo : public FFL::PipelineMessagePayload {
	public:
		virtual void consume() {
		}

		FFL::String mInfo;
	};


	class RefCountWriter : public RefBase {
	public:
		RefCountWriter() {
		}
		virtual ~RefCountWriter() {
			
		}

		virtual IOWriter* getWriter() = 0;
	};

	//
	//  一个writer的message
	//
	class LogWriterMessage : public FFL::PipelineMessagePayload{
	public:
		LogWriterMessage(){
		}
		virtual ~LogWriterMessage() {
		}

		virtual void consume() {
		}

		FFL::sp<RefCountWriter> mWriter;
	};


}

#endif
