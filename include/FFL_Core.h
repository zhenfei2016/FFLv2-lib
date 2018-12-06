/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Core   
*  Created by zhufeifei(34008081@qq.com) on 2017/07/10 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*
*/
#ifndef _FFL_CORE_H_
#define _FFL_CORE_H_

/*  字符串操作 */
#include <string.h>
#include "FFL_Config.h"
#include "FFL_Platform.h"
#ifdef WIN32
/*  c99支持的 */
#include <stdint.h>

#endif

#include "FFL_Utility.h"
#include "FFL_Error.h"
#include "FFL_Assert.h"
#include "FFL_Memory.h"
#include "FFL_Time.h"

/*
 *  socket句柄
 */
typedef int NetFD;
#define INVALID_NetFD 0

#ifdef __cplusplus
#include "utils/FFL_Utils.hpp"
#endif // __cplusplus
#endif
