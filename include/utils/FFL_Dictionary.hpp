/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Dictionary.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/24 
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*
*/
#ifndef _FFL_DICTIONARY_HPP_
#define _FFL_DICTIONARY_HPP_

#include "FFL_String.hpp"
#include <map>

namespace FFL {
	class DictionaryImpl;
	class Dictionary {
	public:
		Dictionary();
		~Dictionary();
		//
		//  写一条key/value
		//
		void setKey(const String& key, const String& value);
		void setKey(const char* key, const String& value);
		//
		// 移除一条
		//
		bool removeKey(const char* key);
		//
		// 通过key获取值,返回是否找到了这个key
		//
		bool getKey(const String& key, String& value);
		bool getKey(const char* key, String& value);
		void getAll(std::map<String,String>& pair);
		
		void setKeyInt64(const String& key,int64_t value);
		void setKeyInt64(const char* key,int64_t value);
		bool getKeyInt64(const String& key, int64_t& value,int64_t def);
		bool getKeyInt64(const char* key, int64_t& value, int64_t def);

		void setKeyInt32(const String& key, int32_t value);
		void setKeyInt32(const char* key, int32_t value);
		bool getKeyInt32(const String& key, int32_t& value, int32_t def);
		bool getKeyInt32(const char* key, int32_t& value, int32_t def);

		Dictionary(const Dictionary & r);
		Dictionary &operator=(const Dictionary & r);
	private:
		mutable DictionaryImpl* mImpl;
	};
}

#endif
