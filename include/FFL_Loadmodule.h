/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Loadmodule   
*  Created by zhufeifei(34008081@qq.com) on 2017/07/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
模块加载函数，
*
*/
#ifndef _FFL_LOADMODULE_H_
#define _FFL_LOADMODULE_H_

#include "FFL_Core.h"
#ifdef  __cplusplus
extern "C" {
#endif

	void* FFL_CALL FFL_loadmodule(const char *file);
	void* FFL_CALL FFL_loadfunction(void *module, const char *fname);
	void  FFL_CALL FFL_unloadmodule(void *module);

#ifdef  __cplusplus
}
#endif

#endif
