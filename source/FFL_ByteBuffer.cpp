/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_ByteBuffer
*  Created by zhufeifei(34008081@qq.com) on 2018/05/06
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  内存管理类
*
*/

#include <FFL_ByteBuffer.hpp>
#include <FFL_ByteStream.hpp>
#include "internalLogConfig.h"

namespace FFL {
	ByteBuffer::ByteBuffer():mData(0),mSize(0), mStream(0){
		mStream = new ByteStream();
		alloc(4096);
	}
	ByteBuffer::~ByteBuffer(){
		FFL_free(mData);		
		mSize = 0;
		FFL_SafeFree(mStream);
	}

	ByteBuffer::ByteBuffer(uint32_t size) :mData(0), mSize(0) {
		mStream = new ByteStream();
		alloc(size);
	}
	//
	// 重新申请一下空间
	//
	uint32_t ByteBuffer::alloc(uint32_t size) {
		if (size <= mSize) {
			return mSize;
		}

		uint8_t* data = (uint8_t*)FFL_malloc(size);
		if (!data) {
			INTERNAL_FFL_LOG_ERROR("ByteBuffer::alloc fail");
			return mSize;
		}	

		FFL_free(mData);
		mData = data;
		mSize = size;

		mStream->setData(mData, 0, mSize);
		return size;
	}
	//
	//  扩大一下内存空间,如果size小于已经申请的则返回以前的大小
	//
	uint32_t ByteBuffer::realloc(uint32_t size) {
		if (size <= mSize) {
			return mSize;
		}

		uint8_t* data=(uint8_t*)FFL_malloc(size);
		if(!data){
			INTERNAL_FFL_LOG_ERROR("ByteBuffer::realloc fail");
			return mSize;
		}

		if (mSize) {
			memcpy(data, mData, mSize);
		}

		FFL_free(mData);
		mData = data;
		mSize = size;

		mStream->setData(mData, 0, mSize);
		return size;
	}

	uint8_t* ByteBuffer::data() const {
		return mData;
	}
	uint32_t ByteBuffer::size() const {
		return mSize;
	}
	//
	//  获取这个内存的操作流
	//
	ByteStream* ByteBuffer::getByteStream() {
		return mStream;
	}
}
