#include <helper/FFL_LogSender.hpp>
#include <pipeline/FFL_PipelineMessageCache.hpp>
#include <net/FFL_NetSocket.hpp>
#include <net/base/FFL_Net.h>

namespace FFL {
#define  LogInfoType 1
#define  ConnSocketType 2
	class LogInfo : public FFL::PipelineMessagePayload {
	public:
		//
		//  已经处理完成了，可以回收了		
		//
		virtual void consume(){
		}

		FFL::String mInfo;
	};

	class LogUploaderNode :public FFL::NodeBase {
	public:
		LogUploaderNode() {
			mSocket = NULL;	
			mIsConnecting = false;
		}
		virtual ~LogUploaderNode() {
		}

		virtual void onCreate() {
			mOutputSocket = createOutputInterface();
			connectInput(mOutputSocket, "conn", NULL);
		}

		virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
			if (msg->getType() == LogInfoType) {
				LogInfo* info=(LogInfo*) (msg->getPayload());
				if (mSocket) {
					size_t writed = 0;
					mSocket->write((void*)(info->mInfo.c_str()), info->mInfo.size(),&writed );
				} else {

					if (!mIsConnecting) {
						FFL::PipelineMessage* controlMsg = new FFL::PipelineMessage(ConnSocketType);
						postMessage(mOutputSocket.getId(), controlMsg);
						mIsConnecting = true;
					}
				}
				msg->consume(this);
			} else if (msg->getType() == ConnSocketType) {
				NetFD fd=0;
				FFL_socketNetworkTcpClient("127.0.0.1",5100,&fd);
				if (fd > 0) {
					mSocket = new CSocket(fd);					
				}
				mIsConnecting = false;
				msg->consume(this);
			}
			return true;
		}

		bool mIsConnecting;
		OutputInterface mOutputSocket;
		CSocket* mSocket;
	};

	class LogPipeline : public FFL::NodeBase {
	public:
		LogPipeline() {
			setName("logInput");
			mMessageCache = new FFL::PipelineMessageCache(LogInfoType);

			mPipeline = new FFL::Pipeline("");
			mManager = new FFL::NodeManager(mPipeline);
		}

		~LogPipeline() {
			FFL_SafeFree(mManager);
		}

		bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {

			FFL::String infoString;
			{
				FFL::CMutex::Autolock l(mInfoLock);
				while (mInfoList.size() > 0) {
					infoString += mInfoList.front() + "\r";
					mInfoList.pop_front();
				}
			}

			if (infoString.size()) {
				LogInfo* info;
				FFL::sp<FFL::PipelineMessage> msg = mMessageCache->createMessage(&info, LogInfoType);
				info->mInfo = infoString;
				if (FFL_OK != postMessage(mOutputUploader.getId(), msg)) {
					msg->consume(this);
				}
			}
			return true;		
		}

		void startup() {
			create(mManager);

			
			connectTimerInput("log-timer", 2000, 0);
			mOutputUploader = createOutputInterface();

			LogUploaderNode* uploader = new LogUploaderNode();
			uploader->setName("logUploader");
			uploader->create(mManager);
			uploader->connectInput(mOutputUploader, "uploader", NULL);

			mPipeline->startup();
		}

		void shutdown() {
			mPipeline->shutdownAndWait();
		}

		void write(uint8_t* data, int32_t len) {
			FFL::String info;
			info = (const char*)data;

			{
				FFL::CMutex::Autolock l(mInfoLock);
				mInfoList.push_back(info);
			}
		}

		FFL::NodeManager* mManager;
		FFL::sp<FFL::Pipeline> mPipeline;

	private:
		FFL::CMutex mInfoLock;
		FFL::List<FFL::String> mInfoList;

		FFL::sp<FFL::PipelineMessageCache> mMessageCache;
		FFL::OutputInterface mOutputUploader;
	};


	LogSender::LogSender() {
		mLogInstance = new LogPipeline();
	}
	LogSender::~LogSender() {
		FFL_SafeFree(mLogInstance);
	}
	void LogSender::startup() {
		mLogInstance->startup();
	}
	void LogSender::shutdown() {
		mLogInstance->shutdown();
	}

	void LogSender::write(uint8_t* data, int32_t len) {
		LogInfo info;
		mLogInstance->write(data, len);
	}	
}


