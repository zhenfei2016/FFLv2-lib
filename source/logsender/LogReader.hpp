#ifndef _FFL_LOG_READEr_HPP_
#define _FFL_LOG_READEr_HPP_


#include <pipeline/FFL_NodeBase.hpp>

namespace FFL {
	class LogReader : public NodeBase {
	public:
		LogReader();
		virtual ~LogReader();

		//
		//  ��reader���������
		//
		void fillData(FFL::String& info);
		//
		//  �����next��
		//
		void outputTo(NodeBase* next, const char* name, void* userdata);
	private:
		//
		//  �ɹ�������node
		//
		virtual void onCreate();

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

	private:
		FFL::CMutex mInfoLock;
		FFL::List< FFL::String> mInfoList;
		FFL::sp<FFL::PipelineMessageCache > mMessageCache;
		OutputInterface mOutputUploader;
	};
		
}


#endif