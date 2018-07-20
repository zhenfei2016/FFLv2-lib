/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_List.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/03
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  Vector封装，先直接继承std库， 换一个名称方便以后的替换
*
*/
#ifndef _FFL_VECTOR_HPP_
#define _FFL_VECTOR_HPP_

#include <FFL.h>
#include <list>
#include <vector>

namespace FFL{	
	template<class VALUE>
	class Vector : public std::vector <VALUE>{
	public:
	};
	
}


#endif

