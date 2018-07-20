/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_List.hpp
*  Created by zhufeifei(34008081@qq.com) on 三月 2018.
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  String封装，先直接用std库， 换一个名称方便以后的替换
*
*/
#ifndef _FFL_STRING_HPP_
#define _FFL_STRING_HPP_

#include <FFL.h>
#include <string>
#include <utils/FFL_Vector.hpp>

namespace FFL {
	
	typedef std::string String;
	int64_t String2int64_t(String& str);
	int32_t String2int32_t(String& str);
	//
	//  格式化字符串
	//
	void formatString(String& str, const char *fmt, ...);
	//
	//  分割字符串
	//
	//FFL::Vector<String> StringSplit(const String& str, const String& flag);
}


#endif

