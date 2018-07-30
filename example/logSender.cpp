#include <FFL.h>
#include <FFL_CommandHelper.h>
#include <helper/FFL_LogSender.hpp>
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

FFL::LogSender* gSender;
int printLogToSender(int level, const char* tag, const char *format, va_list v){	
	char str[4096] = {};
	vsnprintf(str, 4096 - 1, format, v);
	gSender->write((uint8_t*)str,strlen(str));
	return 1;
}

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
	FFL_socketInit();
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	FFL::sp<TestThread> testThread = new TestThread();
	testThread->run("test");

	gSender = new FFL::LogSender();
	gSender->startup();
	FFL_LogHook(printLogToSender);

	//
	//  打印一下帮助函数
	//
	FFL_cmdUsage(gCmdOption);
	FFL_cmdLooper(gCmdOption,0, quitFlag);	
	FFL_sleep(1000);
	return 0;
}
