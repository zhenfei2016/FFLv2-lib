/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Handler.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  参考 Android Open Source Project 进行修改的
*  处理消息的回调函数
*
*/

#include <utils/FFL_Handler.hpp>


namespace FFL {	
	Handler::Handler(): mID(0) {
	}

	Looper::handler_id Handler::id() const{
		return mID;
	}

	IdleHandler::IdleHandler() {
	}
	IdleHandler::~IdleHandler() {
	}


}
