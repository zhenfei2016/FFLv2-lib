/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Loadmodule
*  Created by zhufeifei(34008081@qq.com) on 2017/07/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  模块加载函数，
*
*/

#include "FFL_Loadmodule.h"
#include "FFL_OS.h"
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void* FFL_CALL FFL_loadmodule(const char *file)
{
#ifdef WIN32
	void* hModule = LoadLibraryA(file);
#else
	void* hModule = dlopen(file, RTLD_NOW | RTLD_LOCAL);
#endif
	return hModule;
}
void* FFL_CALL FFL_loadfunction(void *module, const char *fname)
{
#ifdef WIN32
	void *fn = (void *)GetProcAddress((HMODULE)module, fname);
#else
	void *fn = dlsym(module, fname);
#endif
	return fn;
}
void  FFL_CALL FFL_unloadmodule(void *module)
{
#ifdef WIN32
	FreeLibrary((HMODULE)module);
#else
	dlclose(module);
#endif
}
