/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Memory   
*  Created by zhufeifei(34008081@qq.com) on 2017/7/12. 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  内存的申请，释放，大小端控制 ， 内存泄漏检测
*/

#ifndef _FFL_MEMORY_H_
#define _FFL_MEMORY_H_

#include <stdlib.h>
#include <memory.h>


#include "FFL_Utility.h"
#include "FFL_Config.h"

#ifdef  __cplusplus
extern "C" {
#endif
	/*
	 *  内存申请，释放
	 */
	void* FFL_malloc(size_t size);
	void  FFL_free(void *mem);
	/*
	 *  内存申请，并且清空
	*/
	inline static void *FFL_mallocz(size_t size){
		void *mem = FFL_malloc(size);
		if (mem)
			memset(mem, 0, size);
		return mem;
	}
	inline static void FFL_freep(void **mem){
		if (mem && *mem) {
			FFL_free(*mem);
			*mem = 0;
		}
	}

	/*
	 *   打印一下当前还没有释放的内存
	 */
	void  FFL_dumpMemoryLeak();
	/*
	*  打印当前未释放的内存，到文件中
	*/
	void  FFL_dumpMemoryLeakFile(const char* path);
	/*
 	 *  参考上一次释放的内存文件，打印对应的堆栈
	 */
	void  FFL_checkMemoryLeak(const char* path);

	int FFL_outofmemory();
	/*
	 *   memory清零
	 */
	#define FFL_zerop(x) memset((x), 0, sizeof(*(x)))
	#define FFL_zero_array(x,c) memset((x), 0, (sizeof(*(x))*(c)))
	
	#define CHECKED_MALLOC( var, size )\
	do {\
		var = FFL_malloc( size );\
		if( !var ){\
			FFL_LOG_ERROR("alloc memory is null. %s %d ",__FILE__,__LINE__); \
			goto fail;}\
	} while( 0 )

	#define CHECKED_MALLOCZERO( var, size )\
	do {\
	   var = FFL_mallocz( size );\
		if( !var ){\
			FFL_LOG_ERROR("alloc memory is null. %s %d ",__FILE__,__LINE__); \
			goto fail;}\
	} while( 0 )


	char* FFL_CALL FFL_strdup(const char *s);
	char* FFL_CALL FFL_strndup(const char *s, size_t len);

	//
	//  内存字节序
	//

	//
	//大端模式，数据的低位保存在内存的高地址中
	// 网络字节序
	//
    #define  FFL_BIG_ENDIAN 1

	//
	//小端模式，数据的低位保存在内存的低地址中
	//
    #define  FFL_LITTLE_ENDIAN 2

	//
	//  检测系统的大小端
	//
	int FFL_isLittleEndian();
#ifdef  __cplusplus
}
#endif
#endif
