/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Path.h   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/25 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  路径相关的处理类
*/
#ifndef __FFL_PATH_H__
#define __FFL_PATH_H__


#include "FFL_Core.h"
#ifdef  __cplusplus
extern "C" {
#endif
	/*
	 * 获取当前进程的路径，名称
	 */
	status_t FFL_getCurrentProcessPath(char* processdir, size_t len, char* processname);
#ifdef  __cplusplus
}
#endif

#endif
