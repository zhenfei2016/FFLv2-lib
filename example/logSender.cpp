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
static CmdOption  gCmdOption[] = {
	{ "exit",0,quit,"exit sys." },	
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
	logSender.initialize(FFL::LOG_ST_NEW_FILE, "d:\\123.txt");
	//logSender.initialize(FFL::LOG_ST_TCP_CONNECT, "127.0.0.1:5000");
	logSender.startup();
	FFL_hookLogSystem(logSender);
	//
	//  打印一下帮助函数
	//
	FFL_cmdUsage(gCmdOption);
	FFL_cmdLooper(gCmdOption,0, quitFlag);	
	logSender.shutdown();
	FFL_unhookLogSystem();
	FFL_sleep(1000);

	return 0;
}
