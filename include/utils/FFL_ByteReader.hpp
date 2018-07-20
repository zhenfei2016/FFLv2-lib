/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_ByteReader   
*  Created by zhufeifei(34008081@qq.com) on 2018/05/01 
*  https://github.com/zhenfei2016/FFL-v2.git
*  字节流读接口
*/
#ifndef _FFL_BYTE_READER_HPP_
#define _FFL_BYTE_READER_HPP_

#include <FFL.h>
#include "FFL_String.hpp"

namespace FFL{ 
	class ByteReader {	
	public:
		//
		//  字节流读接口
		//
		virtual bool read1Bytes(int8_t& val)=0;
		virtual bool read2Bytes(int16_t& val) = 0;
		virtual bool read3Bytes(int32_t& val) = 0;
		virtual bool read4Bytes(int32_t& val) = 0;
		virtual bool read8Bytes(int64_t& val) = 0;
		virtual bool readString(String& val, uint32_t len) = 0;
		virtual bool readBytes(int8_t* data, uint32_t size) = 0;
	};
}


#endif

