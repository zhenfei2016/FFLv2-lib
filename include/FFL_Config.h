/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Config   
*  Created by zhufeifei(34008081@qq.com) on 2017/07/10 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  编译前生成的配置文件 ,不依赖其他文件的，
*/

#ifndef _FFL_CONFIG_H_
#define _FFL_CONFIG_H_

/*
 *  是否检测内存泄露
 */
#define  CHECK_FOR_MEMORY_LEAKS 0

/*
 *  为了省个事所以加了一个宏定义的判断
 *  正确的做法应该是编译前由配置文件生成的依赖那个config
 */
#ifdef WIN32
#include "config-window.h"
#else


/*  

  使用std库的线程函数  
  
*/
#define FFL_THREAD_STDCPP 0

#define FFL_THREAD_PTHREAD 1

#define HAVE_PTHREADS  1

#endif 


#if defined(FFLIB_COMPILE_SHARED)
/*  函数导出方式  */
#if defined(WIN32)
#define FFLIB_API_IMPORT_EXPORT __declspec(dllexport)
#elif defined(MACOSX) || defined(ANDROID)
#define FFLIB_API_IMPORT_EXPORT __attribute__((visibility("default")))
#else
#define FFLIB_API_IMPORT_EXPORT __attribute__((visibility("default")))
#endif
#elif defined(FFLIB_COMPILE_STATIC)
#define FFLIB_API_IMPORT_EXPORT
#else
/*  函数导入方式  */
#if defined(WIN32)
#define FFLIB_API_IMPORT_EXPORT __declspec(dllimport)
#el
#define FFLIB_API_IMPORT_EXPORT 
#endif

#endif
#endif
