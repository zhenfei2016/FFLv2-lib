/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Log   
*  Created by zhufeifei(34008081@qq.com) on 2017/7/12
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  打印日志帮助类，
*  增加支持的编译日志等级  2017/12/22
*  FFL_LOG_SET_LEVEL_COMPILE = 3  对应FFL_LogLevel的值 
*
*/
#ifndef __FFL_LOG_H__
#define __FFL_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include "FFL_Core.h"
#ifdef  __cplusplus
extern "C" {
#endif	

/************************************************************************
*	支持的日志等级
***********************************************************************/
typedef enum
{
	FFL_LOG_LEVEL_CRIT = 0,
	FFL_LOG_LEVEL_ERROR,
	FFL_LOG_LEVEL_WARNING,
	FFL_LOG_LEVEL_INFO,
	FFL_LOG_LEVEL_DEBUG,
	FFL_LOG_LEVEL_ALL
} FFL_LogLevel;
/************************************************************************
* 设置日志等级
***********************************************************************/
void FFL_LogSetLevel(FFL_LogLevel level);
FFL_LogLevel FFL_LogGetLevel();
const char* FFL_LogGetLevelString(int level);

/************************************************************************
*  设置日志输出到哪里去，外部接管，还是指定文件中
***********************************************************************/
/*
 *  接管日志输出函数
 *  根据返回值表示是否需要继续默认的日志输出
 *  1 : 已经把日志处理了，不需要默认日志系统了
 *  0 : 用默认日志处理函数    
 *  userdata :透传数据
 */
typedef int (*FFL_LogHookFun)(int level,const char* tag,const char *format, va_list args,void* userdata);
void FFL_LogHook(FFL_LogHookFun callback, void* userdata);
/*
 *  设置输出日志到这个文件中
 */
void FFL_LogSetOutput(FILE *file);

/************************************************************************
  通用输出，特定等级输出，特定等级并且带tag的输出
************************************************************************/
void FFL_LogPrint(int level, const char *format, ...);
void FFL_LogPrintTag(int level, const char *tag, const char *format, ...);
void FFL_LogPrintV(int level, const char *format, va_list args);

void FFL_LogCri(const char *format, ...);
void FFL_LogError(const char *format, ...);
void FFL_LogWaring(const char *format, ...);
void FFL_LogInfo(const char *format, ...);
void FFL_LogDebug(const char *format, ...);

void FFL_LogCriTag(const char *tag, const char *format, ...);
void FFL_LogErrorTag(const char *tag, const char *format, ...);
void FFL_LogWaringTag(const char *tag, const char *format, ...);
void FFL_LogInfoTag(const char *tag, const char *format, ...);
void FFL_LogDebugTag(const char *tag, const char *format, ...);

/************************************************************************
*  是否定义了编译日志等级
*  FFL_LOG_SET_LEVEL_COMPILE
************************************************************************/

#if 1

#define FFL_LOG_CRIT(format,...)     FFL_LogPrint(FFL_LOG_LEVEL_CRIT,format,##__VA_ARGS__)
#define FFL_LOG_CRIT_TAG(tag,format,...)     FFL_LogPrintTag(FFL_LOG_LEVEL_CRIT,tag,format,##__VA_ARGS__)

/*
* 是否外部指定了编译时候支持的日志级别，如果没有指定则编译所有等级的日志
*/
#if !defined(FFL_LOG_SET_LEVEL_COMPILE)
#define FFL_LOG_SET_LEVEL_COMPILE FFL_LOG_LEVEL_ALL
#endif

#if  FFL_LOG_SET_LEVEL_COMPILE>=FFL_LOG_LEVEL_ERROR
#define FFL_LOG_ERROR(format,...)            FFL_LogPrint(FFL_LOG_LEVEL_ERROR,format,##__VA_ARGS__)
#define FFL_LOG_ERROR_TAG(tag,format,...)    FFL_LogPrintTag(FFL_LOG_LEVEL_ERROR,tag,format,##__VA_ARGS__)
#endif

#if  FFL_LOG_SET_LEVEL_COMPILE>=FFL_LOG_LEVEL_WARNING
#define FFL_LOG_WARNING(format,...)          FFL_LogPrint(FFL_LOG_LEVEL_WARNING,format,##__VA_ARGS__)
#define FFL_LOG_WARNING_TAG(tag,format,...)  FFL_LogPrintTag(FFL_LOG_LEVEL_WARNING,tag,format,##__VA_ARGS__)
#endif

#if  FFL_LOG_SET_LEVEL_COMPILE>=FFL_LOG_LEVEL_INFO
#define FFL_LOG_INFO(format,...)            FFL_LogPrint(FFL_LOG_LEVEL_INFO,format,##__VA_ARGS__)
#define FFL_LOG_INFO_TAG(tag,format,...)    FFL_LogPrintTag(FFL_LOG_LEVEL_INFO,tag,format,##__VA_ARGS__)
#endif

#if  FFL_LOG_SET_LEVEL_COMPILE>=FFL_LOG_LEVEL_DEBUG
#define FFL_LOG_DEBUG(format,...)           FFL_LogPrint(FFL_LOG_LEVEL_DEBUG,format,##__VA_ARGS__)
#define FFL_LOG_DEBUG_TAG(tag,format,...)   FFL_LogPrintTag(FFL_LOG_LEVEL_DEBUG,tag,format,##__VA_ARGS__)
#endif

#if !defined(FFL_LOG_ERROR)
#define FFL_LOG_ERROR
#define FFL_LOG_ERROR_TAG
#endif

#if !defined(FFL_LOG_WARNING)
#define FFL_LOG_WARNING
#define FFL_LOG_WARNING_TAG
#endif

#if !defined(FFL_LOG_INFO)
#define FFL_LOG_INFO
#define FFL_LOG_INFO_TAG
#endif

#if !defined(FFL_LOG_DEBUG)
#define FFL_LOG_DEBUG
#define FFL_LOG_DEBUG_TAG
#endif

//#define FFL_LOG_CRIT(format,args...)     FFL_LogPrint(FFL_LOG_LEVEL_CRIT,format,args)

#else

#define FFL_LOG_CRIT                 FFL_LogCri
#define FFL_LOG_ERROR                FFL_LogError
#define FFL_LOG_WARNING              FFL_LogWaring
#define FFL_LOG_INFO                 FFL_LogInfo
#define FFL_LOG_DEBUG                FFL_LogDebug

#define FFL_LOG_CRIT_TAG             FFL_LogCriTag
#define FFL_LOG_ERROR_TAG            FFL_LogErrorTag
#define FFL_LOG_WARNING_TAG          FFL_LogWaringTag
#define FFL_LOG_INFO_TAG             FFL_LogInfoTag
#define FFL_LOG_DEBUG_TAG            FFL_LogDebugTag

#endif


#ifdef  __cplusplus
}
#endif
#endif
