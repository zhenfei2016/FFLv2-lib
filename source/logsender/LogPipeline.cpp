#include "LogPipeline.hpp"
#include "LogReader.hpp"
#include "LogUploader.hpp"
#include "LogWriterCreator.hpp"

namespace FFL {	
	LogPipeline::LogPipeline():mReader(NULL){
		mPipeline = new FFL::Pipeline("");
		mManager = new FFL::NodeManager(mPipeline);
	}

	LogPipeline::~LogPipeline() {
		FFL_SafeFree(mManager);
	}

	void LogPipeline::startup() {
		mReader = new LogReader();
		mReader->create(mManager);		
		
		LogUploader* uploader = new LogUploader();
		uploader->create(mManager);		

		LogWriterCreator* writerCreator = new LogWriterCreator();
		writerCreator->create(mManager);
			
		//
		//  reader   ������־���ݸ�uploader
		//  uploader �����������ݸ� writerCreator �ô���writer
		//  writerCreator ����writer�ɹ��󣬷�����uploader��writer
		//
		//  writer:�����ܵ����ݴ洢��ʽ���������磬���صȵ�
		//  ����Ҫ��writer�Ĵ�����ʽ
		//
		mReader->outputTo(uploader, "uploader",0);
		uploader->outputToCreator(writerCreator, "creator-writer", 0);
		writerCreator->outputToWriter(uploader, "update-writer", 0);

		mPipeline->startup();
	}

	void LogPipeline::shutdown() {
		mPipeline->shutdownAndWait();
	}

	void LogPipeline::write(int level,uint8_t* data, int32_t len) {
		char timeFormat[256] = {};
		FFL_getNowString(timeFormat);

		FFL::String info;
		FFL::formatString(info, "%s:%s tid = %d ",
			FFL_LogGetLevelString(level),timeFormat, (int)FFL_CurrentThreadID());
		info.append((const char*)data);
		info.append("\n");

		if (mReader) {
			mReader->fillData(info);
		}
	}

	void LogPipeline::write(int level, const char* tag, const char *format, va_list v) {
		char str[4096] = {};
		vsnprintf(str, 4096 - 1,format, v);
		write(level, (uint8_t*)str, strlen(str));
	}	
}


