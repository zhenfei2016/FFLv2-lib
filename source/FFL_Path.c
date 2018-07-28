/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Path.c   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/25 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  路径处理
*/
#include <FFL_Path.h>

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#if WIN32 
#include <windows.h>
#else
#include <unistd.h>
#endif
/*
* 获取当前进程的路径，名称
*/
status_t FFL_getCurrentProcessPath(char* processdir, size_t len, char* processname) {
	char* pathEnd=0;
    int32_t size=0;
#if WIN32
	GetModuleFileNameA(NULL, processdir, len);
	pathEnd = strrchr(processdir, '\\');
#else	
    getcwd(processdir,len);
    size=strlen(processdir);
    if(size>0 && processdir[size-1]!='/'){
        processdir[size]='/';
        processdir[size+1]=0;
    }
    
    return FFL_OK;
	//if (readlink("/proc/self/exe", processdir, len) <= 0) {
	//	return FFL_FAILED;
	//}
#endif
	if(pathEnd ==NULL)
		pathEnd = strrchr(processdir, '/');
	if(pathEnd == NULL) {
		return FFL_FAILED;
	}

	++pathEnd;
	if (processname) {
		strcpy(processname, pathEnd);
	}
	*pathEnd = '\0';
	return FFL_OK;
	
}

