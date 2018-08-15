/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Memory.c
*  Created by zhufeifei(34008081@qq.com) on 2017/7/12.
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  内存的申请，释放，大小端控制 ， 内存泄漏检测
*/

#include "FFL_Memory.h"
#include "FFL.h"
#if  CHECK_FOR_MEMORY_LEAKS
#include "memoryLeak.c"
#else
#include "memory.c"
#endif


int FFL_outofmemory()
{
	FFL_ASSERT(0);
	FFL_LOG_ERROR("outofmemory");
	return FFL_ERROR_SUCCESS;
}

char* FFL_CALL FFL_strdup(const char *s)
{
	char *ptr = NULL;
	if (s) {
		size_t len = strlen(s) + 1;
		ptr = FFL_mallocz(len);
		if (ptr)
			memcpy(ptr, s, len);
	}
	return ptr;
}

char* FFL_CALL FFL_strndup(const char *s, size_t len)
{
	char *ret = NULL, *end;

	if (!s)
		return NULL;

	end = memchr(s, 0, len);
	if (end)
		len = end - s;

	ret = FFL_mallocz(len + 1);
	if (!ret)
		return NULL;

	memcpy(ret, s, len);
	ret[len] = 0;
	return ret;
}

int FFL_isLittleEndian()
{
	static int littleEndian = -1;
	if (littleEndian == -1) {
		union {
			int32_t i;
			int8_t c;
		} check;

		check.i = 0x01;
		littleEndian = check.c;
	}
	return littleEndian;
}