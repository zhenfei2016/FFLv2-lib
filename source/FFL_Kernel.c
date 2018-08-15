
#include <FFL.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
/*
*  获取进程id
*/
int32_t FFL_getProcessId() {
	int32_t id = 0;
#ifdef WIN32
	id=(int32_t)GetCurrentProcessId();
#else
	pid_t pid=getpid();
	id = pid;
#endif 

	return id;
}