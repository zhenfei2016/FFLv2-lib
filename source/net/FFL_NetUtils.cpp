/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetUtils.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  帮组函数
*
*/

#include <utils/FFL_StringHelper.hpp>

namespace FFL {

	void FFL_parseHostport(const String& url, String& host, int32_t& port){
		size_t pos = url.find(":");
		if (pos != std::string::npos) {
			String p = url.substr(pos + 1);
			host = url.substr(0, pos);
			port = ::atoi(p.c_str());
		}else {
			host = url;
		}
	}

	//
	//  是否一个ip, 是的话返回FFL_Ok
	//
	status_t FFL_isIp(const String& ip) {
		return ip.size() > 0 ?FFL_OK:FFL_FAILED;
	}
}