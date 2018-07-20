/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Initialize   
*  Created by zhufeifei(34008081@qq.com) on 2018/01/17
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  FFL 全局初始化，结束函数定义，实现
*
*/
#ifndef _FFL_INITIALIZE_H_
#define _FFL_INITIALIZE_H_

#include "FFL_Core.h"
#ifdef  __cplusplus
extern "C" {
#endif

	/*
	 *  初始化函数
	 *
	 * */
	extern DECLSPEC void FFL_CALL FFL_initialize();


	/*
	 *  结束FFL库的使用，调用后就不要再使用FFL的函数了
	 *
	 * */
	extern DECLSPEC void FFL_CALL FFL_terminate();


#ifdef  __cplusplus
}
#endif

#endif
