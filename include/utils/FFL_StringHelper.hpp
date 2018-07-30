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
#ifndef _FFL_STRING_HELPER_HPP_
#define _FFL_STRING_HELPER_HPP_

#include <FFL.h>

#include <utils/FFL_String.hpp>
#include <utils/FFL_Vector.hpp>

namespace FFL {
	
	//
	//  分割字符串
	//
	FFL::Vector<String> StringSplit(const String& str, const String& flag);

	//
	//  trim一个字符
	//
	String stringTrimLeft(const String& str, uint8_t ch);
	String stringTrimRight(const String& str, uint8_t ch);

	//
	//  以什么结尾，开头
	//
	bool stringStartsWith(String& str, const String& prefix);
	bool stringEndsWith(String& str, const String& subfix);

	//
	//  转大小写
	//
	String stringUpper(const String& str);
	String stringLower(const String& str);
}


#endif

