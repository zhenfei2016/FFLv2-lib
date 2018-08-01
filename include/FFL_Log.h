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
打印日志帮助类
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
	

/*
 *	支持的日志等级
*/
typedef enum
{
	FFL_LOG_LEVEL_CRIT = 0,
	FFL_LOG_LEVEL_ERROR,
	FFL_LOG_LEVEL_WARNING,
	FFL_LOG_LEVEL_INFO,
	FFL_LOG_LEVEL_DEBUG,
	FFL_LOG_LEVEL_ALL
} FFL_LogLevel;

/*
	日志输出宏
	*/
#define FFL_LOG_CRIT                 FFL_log_cri
#define FFL_LOG_ERROR                FFL_log_error
#define FFL_LOG_WARNING              FFL_log_waring
#define FFL_LOG_INFO                 FFL_log_info
#define FFL_LOG_DEBUG                FFL_log_debug

#define FFL_LOG_CRIT_TAG             FFL_log_cri_tag
#define FFL_LOG_ERROR_TAG            FFL_log_error_tag
#define FFL_LOG_WARNING_TAG          FFL_log_waring_tag
#define FFL_LOG_INFO_TAG             FFL_log_info_tag
#define FFL_LOG_DEBUG_TAG            FFL_log_debug_tag


/*
 * 设置显示的日志等级
 *
*/
void FFL_LogSetLevel(FFL_LogLevel level);
FFL_LogLevel FFL_LogGetLevel();

const char* FFL_LogGetLevelString(int level);
/*
 *  设置输出日志hook函数
 *  根据返回值表示是否需要继续默认的日志输出
 *  1 : 已经把日志处理了，不需要默认日志系统了
 *  0 : 用默认日志处理函数    
 *  userdata :透传数据
 */
typedef int (*FFL_LogHookFun)(int level,const char* tag,const char *format, va_list args,void* userdata);
void FFL_LogHook(FFL_LogHookFun callback, void* userdata);


/*
*日志文件
*/
void FFL_LogSetOutput(FILE *file);

/*
*输出函数
*/
void FFL_log_cri(const char *format, ...);
void FFL_log_error(const char *format, ...);
void FFL_log_waring(const char *format, ...);
void FFL_log_info(const char *format, ...);
void FFL_log_debug(const char *format, ...);


void FFL_log_cri_tag(const char *tag, const char *format, ...);
void FFL_log_error_tag(const char *tag, const char *format, ...);
void FFL_log_waring_tag(const char *tag, const char *format, ...);
void FFL_log_info_tag(const char *tag, const char *format, ...);
void FFL_log_debug_tag(const char *tag, const char *format, ...);

#ifdef  __cplusplus
}
#endif

#endif
