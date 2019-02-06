/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Version   
*  Created by zhufeifei(34008081@qq.com) on 2018/1/7. 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  版本相关信息 ,参考的ffmpeg中的宏定义，基本都是copy的
*
*/
#ifndef _FFL_VERSION_H_
#define _FFL_VERSION_H_

#include <FFL_Core.h>

#define FFL_VERSION_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define FFL_VERSION_DOT(a, b, c) FFL_TOSTRING(a)##"."## FFL_TOSTRING(b) ##"."## FFL_TOSTRING(c)

/*
 *  对应版本号的3段
 *  
*/
#define FFL_VERSION_MAJOR  3
#define FFL_VERSION_MINOR  0
#define FFL_VERSION_MICRO  0

/*
   整形的版本号  ，版本号分2段，每段8字节的(0-255)
     
*/
#define FFLLIB_VERSION_INT     FFL_VERSION_INT(FFL_VERSION_MAJOR,FFL_VERSION_MINOR,FFL_VERSION_MICRO)

/*
*  字符串类型版本号 0.1.1  
*/
#define FFLLIB_VERSION_STRING  FFL_VERSION_DOT(FFL_VERSION_MAJOR,FFL_VERSION_MINOR,FFL_VERSION_MICRO)

#ifdef  __cplusplus
extern "C" {
#endif
/*
*  获取版本号，字符串类型
*/
FFLIB_API_IMPORT_EXPORT const char* FFL_CALL FFL_GetVersion();

/*
 *  获取版本号，整形
 */
FFLIB_API_IMPORT_EXPORT int FFL_CALL FFL_GetVersion_int();

#ifdef  __cplusplus
}
#endif


#endif
