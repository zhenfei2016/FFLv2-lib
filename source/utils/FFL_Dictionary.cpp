/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Dictionary.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/24 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*
*/
#include <utils/FFL_String.hpp>
#include <utils/FFL_Dictionary.hpp>
#include <map>

namespace FFL{
	typedef std::map<String, String> DIC_MAP;
	typedef DIC_MAP::iterator PDIC_MAP;

	class DictionaryImpl {
	public:
		DictionaryImpl(){}
		~DictionaryImpl(){}

		DIC_MAP mDic;
	};

	Dictionary::Dictionary(){
		mImpl = new DictionaryImpl();
	}
	Dictionary::~Dictionary(){
		FFL_SafeFree(mImpl);
	}

	//
	//  写一条key/value
	//
	void Dictionary::setKey(const String& key, const String& value){	
		mImpl->mDic[key] = value;
	}
	void Dictionary::setKey(const char* key, const String& value) {
		mImpl->mDic[key] = value;
	}
	//
	// 通过key获取值,返回是否找到了这个key
	//
	bool Dictionary::getKey(const String& key, String& value) {
		PDIC_MAP pos = mImpl->mDic.find(key);
		if (pos != mImpl->mDic.end()) {
			value= pos->second;
			return true;
		}
		return false;
	}
	bool Dictionary::getKey(const char* key, String& value) {
		PDIC_MAP pos = mImpl->mDic.find(key);
		if (pos != mImpl->mDic.end()) {
			value = pos->second;
			return true;
		}
		return false;
	}
	//
	// 移除一条
	//
	bool Dictionary::removeKey(const char* key) {
		PDIC_MAP pos = mImpl->mDic.find(key);
		if (pos != mImpl->mDic.end()) {
			mImpl->mDic.erase(pos);
			return true;
		}
		return false;
	}

	void Dictionary::setKeyInt64(const String& key, int64_t value) {
		String strVal;
		formatString(strVal, "%" lld64, value);
		setKey(key, strVal);
	}
	void Dictionary::setKeyInt64(const char* key, int64_t value) {
		String strVal;
		formatString(strVal, "%" lld64, value);
		setKey(key, strVal);
	}
	bool Dictionary::getKeyInt64(const String& key, int64_t& value, int64_t def) {
		String strVal;
		if (getKey(key, strVal)) {
			value = String2int64_t(strVal);
			return true;
		}
		value = def;
		return false;
	}
	bool Dictionary::getKeyInt64(const char* key, int64_t& value, int64_t def) {
		String strVal;
		if (getKey(key, strVal)) {
			value = String2int64_t(strVal);
			return true;
		}
		value = def;
		return false;
	}

	void Dictionary::setKeyInt32(const String& key, int32_t value) {
		String strVal;
		formatString(strVal, "%d", value);
		setKey(key, strVal);
	}
	void Dictionary::setKeyInt32(const char* key, int32_t value) {
		String strVal;
		formatString(strVal, "%d", value);
		setKey(key, strVal);
	}
	bool Dictionary::getKeyInt32(const String& key, int32_t& value, int32_t def) {
		String strVal;
		if (getKey(key, strVal)) {
			value = String2int32_t(strVal);
			return true;
		}
		value = def;
		return false;
	}		
	bool Dictionary::getKeyInt32(const char* key, int32_t& value, int32_t def) {
		String strVal;
		if (getKey(key, strVal)) {
			value = String2int32_t(strVal);
			return true;
		}
		value = def;
		return false;
	}

}

