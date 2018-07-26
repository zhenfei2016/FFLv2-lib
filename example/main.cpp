#include <stdio.h>
#include <utils/FFL_MemoryWatch.hpp>

const char* ShowLogTag[] = {
	//"audio",
	//"timestamp",
	//"MsgQueue",
	0
};

#if WIN32
#include <windows.h>
int printLog(int level,const char* tag, const char *format, va_list v)
{	
	bool showLog = false;
	if (level <= FFL_LOG_LEVEL_WARNING) {
		showLog = true;
	}else if ( tag) {		
		for (int i = 0; i < FFL_ARRAY_ELEMS(ShowLogTag); i++) {
			if (ShowLogTag[i] && strcmp(tag, ShowLogTag[i]) == 0) {
				showLog = true;
				break;
			}
		}		
	}

	if (showLog) {
		char str[1024] = {};
		vsnprintf(str + 2, 1024 - 1, format, v);
		str[0] = ' ';
		str[1] = ' ';

		char timeStr[128] = { 0 };
		FFL_getNowString(timeStr);
		::OutputDebugStringA(timeStr);
		::OutputDebugStringA(str);
		::OutputDebugStringA("\r\n");		
	}

	return 1;
}

int printLogAll(int level, const char* tag, const char *format, va_list v)
{	
	char str[1024] = {};
	vsnprintf(str + 2, 1024 - 1, format, v);
	str[0] = ' ';
	str[1] = ' ';

	char timeStr[128] = { 0 };
	FFL_getNowString(timeStr);
	::OutputDebugStringA(timeStr);
	::OutputDebugStringA(str);
	::OutputDebugStringA("\r\n");
	return 1;
}


#else
int printLog(int level,const char* tag, const char *format, va_list v)
{
    if ( tag) {
        if(strcmp(tag, "timestamp") == 0 ) {
            return  0;
        }
 
    }
    return  1;
}
#endif
extern int FFL_main();

extern "C" int main(int argc ,const char* argv[]) {	
	FFL::startMemoryWatch();
#if WIN32
//	FFL_LogHook(printLogAll);
#endif
    FFL_LogSetLevel(FFL_LOG_LEVEL_ALL);
	//FFL_LogSetLevel(FFL_LOG_LEVEL_WARNING);
	FFL_LOG_INFO("start server");
	FFL_main();
	FFL_LOG_INFO("quit server");
	FFL_dumpMemoryLeak();
	getchar();
	return 0;
}
