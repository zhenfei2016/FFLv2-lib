/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Initialize
*  Created by zhufeifei(34008081@qq.com) on 2018/01/17
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  FFL 全局初始化，结束函数定义，实现
*
*/
#include "FFL_Initialize.h"
#include "FFL.h"

/*
 *  初始化函数
 *
 * */
void FFL_CALL FFL_initialize()
{
    FFL_LOG_INFO("FFL_initialize call");
}


/*
 *  结束FFL库的使用，调用后就不要再使用FFL的函数了
 *
 * */
void FFL_CALL FFL_terminate()
{
    FFL_LOG_INFO("FFL_terminate call");
}

