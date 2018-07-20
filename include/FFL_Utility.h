/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Utility   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/10 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  定义了一系列的公用宏
*/
#ifndef _FFL_UTILITY_H_
#define _FFL_UTILITY_H_

#include "FFL_Platform.h"

#ifndef DECLSPEC
/*  函数导出方式  */
#if defined(WIN32)
#define DECLSPEC __declspec(dllexport)
#elif defined(MACOSX) || defined(ANDROID)
#define DECLSPEC __attribute__((visibility("default")))
#else
#define DECLSPEC __attribute__((visibility("default")))
#endif
#endif
/*
 *打印64位整形
 * print("hi%"lld64, 123445 );
 */
#if defined(WIN32)
#define lld64 "I64d"
#else
#define lld64 "lld"
#endif

/* 
 *  函数调用方式
 */
#ifndef FFL_CALL
#if (defined(WIN32))
#define FFL_CALL __cdecl
#elif (defined(MACOSX))
#define FFL_CALL
#else
#define FFL_CALL
#endif
#endif

/*
* 内联函数
*/
#define FFL_INLINE inline


/*  最大值 */
#ifndef FFL_MAX
#define FFL_MAX(a, b)    ((a) > (b) ? (a) : (b))
#endif

/*  最小值 */
#ifndef FFL_MIN
#define FFL_MIN(a, b)    ((a) < (b) ? (a) : (b))
#endif

/*  align字节对齐 */
#ifndef FFL_ALIGN
#define FFL_ALIGN(x, align) ((( x ) + (align) - 1) / (align) * (align))
#endif

/*   
   如果condition__条件满足了，则返回retval_值
*/
#define FFL_CHECK_RET(condition__, retval__)   if (!(condition__)) { return (retval__); }

/* 数组元素个数 */
#ifndef FFL_ARRAY_ELEMS
#define FFL_ARRAY_ELEMS(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

/* 交换2个数值 */
#ifndef FFL_SWAP
#define FFL_SWAP(type,a,b) do{type tmp= b; b= a; a= tmp;}while(0)
#endif

/*
 *  转换到字符串类型
*/
#ifndef FFL_TOSTRING
#define FFL_TOSTRING(s) #s
#endif

#ifndef FFL_INT32_HIGHT_16
#define FFL_INT32_HIGHT_16(a) ((((int)(a))>>16) & 0xFFFF)
#endif

#ifndef FFL_INT32_LOW_16
#define FFL_INT32_LOW_16(a) ((((int)(a))) & 0xFFFF)
#endif

#ifndef FFL_MAKE_INT32
#define FFL_MAKE_INT32(high,low) (((low)& 0xFFFF) | (((high)& 0xFFFF)<<16))
#endif


#ifndef FFL_INT64_HIGHT_32
#define FFL_INT64_HIGHT_32(a) ((int32_t)( ((a) >> 32) & 0xFFFFFFFF))
#endif

#ifndef FFL_INT64_LOW_32
#define FFL_INT64_LOW_32(a) ((int32_t)( (a) & 0xFFFFFFFF ))
#endif

#ifndef FFL_MAKE_INT64
#define FFL_MAKE_INT64(high,low) (((int64_t)(low)& 0xFFFFFFFF) | (((int64_t)(high)& 0xFFFFFFFF)<<32))
#endif

/*
 *  c中定义struct结构体，模拟class定义
 *
*/
#define FFL_DECLEAR_CLASS(type)  \
         type* FFL_Allocate_##type(); \
         void FFL_Free_##type(type* p);

#define FFL_IMPLEMENT_CLASS(type,contruct_fun) \
        type* FFL_Allocate_##type() {	\
		type* p = (type*)FFL_mallocz(sizeof(type)); \
		if (p == 0) {\
			 FFL_LOG_WARNING("Allocate %s failed.", #type); return 0;}  \
             if(contruct_fun){ \
                contruct_fun(p);} \
             return p;}\
         \
         void FFL_Free_##type(type* p){ \
			if ((p)&&(p)->Destruct){ \
				(p)->Destruct((p)); \
                FFL_free((p));}} \



#define FFL_DECLEAR_CLASS_EX(type)  \
         type* FFL_Allocate_##type(const char*); \
         void FFL_Free_##type(type* p);

#define FFL_IMPLEMENT_CLASS_EX(type,contruct_fun) \
        type* FFL_Allocate_##type(const char* name) {	\
		type* p = (type*)FFL_mallocz(sizeof(type)); \
		if (p == 0) {\
			 FFL_LOG_WARNING("Allocate %s failed.", #type); return 0;}  \
             if(contruct_fun){ \
                contruct_fun(p,name);} \
             return p;}\
         \
         void FFL_Free_##type(type* p){ \
			if ((p)&&(p)->Destruct){ \
				(p)->Destruct((p)); \
                FFL_free((p));}} \



#ifdef __cplusplus
extern "C" {
#endif
	/*
	*  生成一个唯一的id
	*/
	int FFL_generateId();
#ifdef __cplusplus
}
#endif


#endif
