/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_MemoryWatch
*  Created by zhufeifei(34008081@qq.com) on 2018/03/18
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  启用内存监控，可以监控new,deletec 操作
*/

#ifndef _FFL_VECTOR_HPP_
#define _FFL_VECTOR_HPP_

#include <FFL.h>

static bool gStartWatch = true;

#if  CHECK_FOR_MEMORY_LEAKS
//
//替代编译器全局默认的operator函数
//
void * operator new(size_t size) {	
	void * ptr = 0;
	if(gStartWatch)
	   ptr = FFL_malloc(size);
	else
	   ptr = malloc(size);
	return ptr;
}
//
//替换全局的operator delete 函数
//
void operator delete (void * ptr) {	
	if (gStartWatch)
		FFL_free(ptr);
	else
		free(ptr);
	ptr = nullptr;
}

#endif

namespace FFL{	
	void startMemoryWatch() {
		FFL_LOG_INFO("startMemoryWatch");
		gStartWatch = true;
	}
}


#endif

