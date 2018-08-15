#include <FFL.h>
#include <FFL_CommandHelper.h>
#include <logsender//FFL_LogSender.hpp>
#include <thread/FFL_Thread.hpp>
#include <net/base/FFL_Net.h>

static int gExitFlag = 0;
void quit(const char* args, void* userdata) {
	
	gExitFlag = 1;
	printf("exit sys. \n");
}
static int quitFlag(void* userdata) {
	return gExitFlag;
}

static int gFilePathIndex = 1;
void setTarget(const char* args, void* userdata) {
	FFL::LogSender* logSender = (FFL::LogSender*) userdata;
	char path[1024] = {};
	sprintf(path, "d:\\logsender_%d.txt", gFilePathIndex++);
	logSender->setTargetUrl(FFL::LOG_ST_NEW_FILE, path);
}

static CmdOption  gCmdOption[] = {
	{ "exit",0,quit,"exit sys." },	
	{ "setTarget",0,setTarget,"set log Target file path." },
	{ 0,0,0,0 },
};


static int32_t gIndex = 0;
class TestThread : public FFL::Thread {
public:
	virtual bool threadLoop() {
		FFL_LOG_WARNING("test =%d", gIndex++);
		FFL_sleep(1000 * 5);
		return gExitFlag!=1;
	}
};
int FFL_main() {
	//
	//  模拟日志输入
	//
	FFL::sp<TestThread> testThread = new TestThread();
	testThread->run("test");

	//
	//
	//
	FFL::LogSender logSender;	
	logSender.setTargetUrl(FFL::LOG_ST_NEW_FILE, "d:\\logsender_0.txt");
	//logSender.setTargetUrl(FFL::LOG_ST_TCP_CONNECT, "127.0.0.1:5000");
	logSender.startup();
	FFL_hookLogSystem(logSender);
	//
	//  打印一下帮助函数
	//
	FFL_cmdUsage(gCmdOption);
	FFL_cmdLooper(gCmdOption,&logSender, quitFlag);
	logSender.shutdown();
	testThread->requestExitAndWait();
	FFL_unhookLogSystem();	

	return 0;
}
