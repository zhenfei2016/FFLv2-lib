/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Log
*  Created by zhufeifei(34008081@qq.com) on 2017/7/12
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  打印日志帮助类
*
*/
#include <stdio.h>
#include <stdarg.h>

#include "FFL_Log.h"
#include "FFL_Time.h"
#include "thread/FFL_Thread.h"

/*
 *  默认情况下打印的最大日志等级
 *  debug模式下打印所有日志
 */
#ifdef _DEBUG
static FFL_LogLevel gLogMaxLevel = FFL_LOG_LEVEL_ALL;
#else
static FFL_LogLevel gLogMaxLevel = FFL_LOG_LEVEL_ERROR;
#endif 

/*
 *  默认情况下打印日志函数
 */
static int defaultPrintLog(int level,const char* tag, const char *format, va_list vl);
static FFL_LogHookFun gLogHookFun = NULL;

static FILE *gLogFd;


/*
 *  所有的打印函数入口
 */
static void internalPrintLog(int level,const char* tag, const char *format, va_list args)
{
	if (level <= gLogMaxLevel) {
		if (gLogHookFun && (gLogHookFun(level, tag, format, args) > 0)) {
			return;
		}

		/*
		 * 默认日志处理函数
		 */
		defaultPrintLog(level, tag, format, args);
	}
}

static const char *s_levels_string[] = {
  "CRI",
  "ERR",
  "WAR",
  "INF",
  "DBG"
};

void FFL_LogSetLevel(FFL_LogLevel level)
{
	gLogMaxLevel = level;
}
FFL_LogLevel FFL_LogGetLevel()
{
	return gLogMaxLevel;
}
const char* FFL_LogGetLevelString(int level) {
	if (level < 0 || level >= FFL_LOG_LEVEL_ALL) {
		return "NUL";
	}

	return s_levels_string[level];
}

void FFL_LogSetOutput(FILE *file)
{
	gLogFd = file;
}

void FFL_LogHook(FFL_LogHookFun cb)
{
	gLogHookFun = cb;
}

/*
*  每条的输出日志日志最长
*/
#define MAX_LOG_LENGTH	2048
static int defaultPrintLog(int level,const char* tag, const char *format, va_list vl)
{
	char str[MAX_LOG_LENGTH] = {0};
	char timeFormat[64] = { 0 };	
	if (level > gLogMaxLevel) {
		return 1;
	}

	vsnprintf(str, MAX_LOG_LENGTH - 1, format, vl);
	if ( !gLogFd) 
		gLogFd = stdout;
	
	{
		FFL_getNowString(timeFormat);
#ifdef WIN32
		fprintf(gLogFd, "%s:%s tid=%d %s \n", s_levels_string[level], timeFormat, (int)FFL_CurrentThreadID(),  str);	
#else
        fprintf(gLogFd, "%s:%s tid=%d %s \n", s_levels_string[level], timeFormat, (int)FFL_CurrentThreadID(),  str);
#endif
#ifdef _DEBUG
		fflush(gLogFd);
#endif
	}
	return 1;
}

void FFL_log_cri(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_CRIT, 0,format, args);
	va_end(args);
}

void FFL_log_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_ERROR, 0, format, args);
	va_end(args);
}
void FFL_log_waring(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_WARNING, 0, format, args);
	va_end(args);
}
void FFL_log_info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_INFO, 0, format, args);
	va_end(args);
}
void FFL_log_debug(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_DEBUG, 0, format, args);
	va_end(args);
}

/*
 *   带tag标志的日志函数
 */
void FFL_log_cri_tag(const char *tag,const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_CRIT, tag, format, args);
	va_end(args);
}
void FFL_log_error_tag(const char *tag,const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_ERROR, tag, format, args);
	va_end(args);
}
void FFL_log_waring_tag(const char *tag,const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_WARNING, tag, format, args);
	va_end(args);
}
void FFL_log_info_tag(const char *tag,const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_INFO, tag, format, args);
	va_end(args);
}
void FFL_log_debug_tag(const char *tag,const char *format, ...)
{
	va_list args;
	va_start(args, format);
	internalPrintLog(FFL_LOG_LEVEL_DEBUG, tag, format, args);
	va_end(args);
}
