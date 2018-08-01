#ifndef _FFL_LOG_WRITER_CREATOR_HPP_
#define _FFL_LOG_WRITER_CREATOR_HPP_


#include <pipeline/FFL_NodeBase.hpp>
#include <utils/FFL_Io.hpp>

namespace FFL {
	class LogWriterCreator :public FFL::NodeBase {
	public:
		LogWriterCreator();
		virtual ~LogWriterCreator();

		//
		//  输出一个新的writer
		//
		void outputToWriter(NodeBase* next, const char* name, void* userdata);
	protected:
		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
	private:
		void createNetWriter();
	private:
		bool mIsConnecting;
		OutputInterface mOutputWriter;
		IOWriter* mWriter;
	};
}

#endif