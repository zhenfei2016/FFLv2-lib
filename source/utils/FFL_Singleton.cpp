/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Singleton.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/23 
*  https://github.com/zhenfei2016/FFL-v2.git
*  单例 
*
*/
#include <utils/FFL_Singleton.hpp>
namespace FFL {
	CMutex gSingletonMutex;
	CMutex& getSingletonLock() {
		return gSingletonMutex;
	}
}


