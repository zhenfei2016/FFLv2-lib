/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Utility.c
*  Created by zhufeifei(34008081@qq.com) on 2018/06/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*/
#include <FFL.h>
#include "atomic/atomic.h"
static AtomicInt g_atomicId;
static int gFirst=1;

int FFL_generateId(){
    if(gFirst){
       FFL_atomicInit(&g_atomicId,1);
        gFirst=0;
    }
	return atomicInc(&g_atomicId);
}
