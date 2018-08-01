#ifndef _FFL_LOG_UPLOADER_HPP_
#define _FFL_LOG_UPLOADER_HPP_

#include <pipeline/FFL_NodeBase.hpp>
#include <net/FFL_NetSocket.hpp>


namespace FFL {
	class RefCountWriter;
	class LogUploader :public FFL::NodeBase {
	public:
		LogUploader();
		virtual ~LogUploader();
		//
		//  输出到next中,请求一个creator
		//
		void outputToCreator(NodeBase* next, const char* name, void* userdata);
	protected:	
		void onCreate();

		FFL::sp<FFL::PipelineConnector > onCreateConnector(	const OutputInterface& output,
			const InputInterface& input,
			void* userdata) ;

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
		//
		//  处理日志消息
		//
		void handleLogInfo(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
		//
		//  更新writer
		//
		void handleUpdateWriter(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
	private:
		//
		//  创建writer
		//
		void tryCreateWriter();
	private:		
		OutputInterface mOutputWriter;

		//
		//  上次创建writer的时间
		//
		int64_t mCreateWriterTime;
		FFL::sp<FFL::RefCountWriter> mWriter;

		FFL::sp<FFL::PipelineLooper > mLooper;
	};
}

#endif




