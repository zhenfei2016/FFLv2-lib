#include <FFL.h>
#include <helper/FFL_NodeBase.hpp>
#include <FFL_CommandHelper.h>


class LogInputNode : public FFL::NodeBase {
public:
	//
	//  成功创建了node
	//
	virtual void onCreate() {

	}
	//
	//  成功删除了node
	//
	virtual void onDestroy() {

	}
	virtual void onStartMessageLooper() {

	}
	virtual void onQuitMessageLooper() {

	}

	virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		return true;
	}
};

class LogUploaderNode :public FFL::NodeBase {
public:
	//
	//  成功创建了node
	//
	virtual void onCreate() {

	}
	//
	//  成功删除了node
	//
	virtual void onDestroy() {

	}
	virtual void onStartMessageLooper() {

	}
	virtual void onQuitMessageLooper() {

	}

	virtual bool onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		return true;
	}
};

class LogPipeline{
public:
	LogPipeline() {
		mPipeline = new FFL::Pipeline("");
		mManager = new FFL::NodeManager(mPipeline);
	}

	~LogPipeline() {
		FFL_SafeFree(mManager);
	}

	void startup() {
		LogInputNode* node = new LogInputNode();
		node->setName("logInput");
		node->create(mManager);
		FFL::OutputInterface output=node->createOutputInterface();

		LogUploaderNode* uploader = new LogUploaderNode();
		uploader->setName("logUploader");
		uploader->create(mManager);
		uploader->connectInput(output, "uploader",NULL);

		mPipeline->startup();
	}

	void shutdown() {
		mPipeline->shutdownAndWait();
	}

	FFL::NodeManager* mManager;
	FFL::sp<FFL::Pipeline> mPipeline;
};

static int gExitFlag = 0;
void quit(const char* args, void* userdata) {
	LogPipeline* srv =(LogPipeline*) userdata;
	srv->shutdown();

	gExitFlag = 1;
	printf("exit sys. \n");
}
static int quitFlag(void* userdata) {
	return gExitFlag;
}
static CmdOption  gCmdOption[] = {
	{ "exit",0,quit,"exit sys." },	
	{ 0,0,0,0 },
};

int FFL_main() {
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	LogPipeline logServer;
	logServer.startup();

	//
	//  打印一下帮助函数
	//
	FFL_cmdUsage(gCmdOption);
	FFL_inputLooper(gCmdOption,&logServer, quitFlag);
	
	FFL_sleep(1000);
	return 0;
}
