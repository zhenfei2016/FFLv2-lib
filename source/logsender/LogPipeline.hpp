
#include <pipeline/FFL_NodeBase.hpp>
#include "LogReader.hpp"

namespace FFL {
	class WriterConfigure {
	public:

	};

	class LogPipeline  {
	public:
		LogPipeline();
		~LogPipeline();	
		//
		//  ������ֹͣLogPipeline
		//
		void startup();
		void shutdown();        
		//
		//  д��־��Ϣ
		//
		void write(int level, uint8_t* data, int32_t len);
		void write(int level, const char* tag, const char *format, va_list v);
	private:
		FFL::NodeManager* mManager;
		FFL::sp<FFL::Pipeline> mPipeline;
		//
		//  ��־�б�
		//
		FFL::CMutex mInfoLock;
		FFL::List<FFL::String> mInfoList;

		LogReader* mReader;
	};
}


