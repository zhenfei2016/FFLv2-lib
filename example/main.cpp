#include <utils/FFL_MemoryWatch.hpp>

extern int FFL_main();
extern "C" int main(int argc ,const char* argv[]) {	
	FFL::startMemoryWatch();
    FFL_LogSetLevel(FFL_LOG_LEVEL_ALL);	
	FFL_main();	
	FFL_dumpMemoryLeak();

	printf("press any key to quit");
	getchar();
	return 0;
}
