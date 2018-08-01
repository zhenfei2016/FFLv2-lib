#include "LogWriterCreator.hpp"
#include "LogMessageId.hpp"
#include "LogMessages.hpp"
#include <net/base/FFL_Net.h>
#include <net/FFL_NetSocket.hpp>

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


	LogWriterCreator::LogWriterCreator() {
		setName("LogWriterCreator");
		mWriter = NULL;		
	}

	LogWriterCreator:: ~LogWriterCreator() {
	}

	void LogWriterCreator::outputToWriter(NodeBase* next, const char* name, void* userdata) {
		mOutputWriter = connectNext(next, name, userdata);
	}

	bool LogWriterCreator::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		if (msg->getType() == LOG_MESSAGE_CREATE_WRITER) {
			createNetWriter();
		}
		msg->consume(this);
		return true;
	}

	void LogWriterCreator::createNetWriter() {
		NetFD fd = 0;
		FFL_socketNetworkTcpClient("127.0.0.1", 5100, &fd);

		FFL::PipelineMessage* controlMsg = new FFL::PipelineMessage(LOG_MESSAGE_UPDATE_WRITER);
		LogWriterMessage* msg = new LogWriterMessage();
		if (fd > 0) {
			msg->mWriter = new NetWriter(fd);
			
		}else {
			msg->mWriter = NULL;
		}

		controlMsg->setPayload(msg);
		postMessage(mOutputWriter.getId(), controlMsg);
	}
}


