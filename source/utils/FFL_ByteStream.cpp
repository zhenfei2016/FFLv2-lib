/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_ByteStream   
*  Created by zhufeifei(34008081@qq.com) on 2018/05/1
*  https://github.com/zhenfei2016/FFL-v2.git
*
*
*/
#include <utils/FFL_ByteStream.hpp>

namespace FFL {
	ByteStreamBase::ByteStreamBase():mMemEndian(0){
		setData(0, 0,0);
	}
	ByteStreamBase::~ByteStreamBase() {
	}
	//
	//  设置数据，bytestream将在这个数据集上进行操作
	//
	void ByteStreamBase::setData(uint8_t* data, uint32_t size, uint32_t capticity) {
		setData(data,
			size,
			capticity,
			FFL_isLittleEndian() ? FFL_LITTLE_ENDIAN : FFL_BIG_ENDIAN);
	}

	void ByteStreamBase::setData(uint8_t* data, uint32_t size, uint32_t capticity, int memEndian) {
		FFL_ASSERT(capticity >= size);
		mData = data;
		mDataCapacity = capticity;
		mDataSize = size;
		mMemEndian = memEndian;
	}	
	//
	//  是否系统的字节序与mData中的一致
	//
	bool ByteStreamBase::isSameEndian() {
		if (FFL_isLittleEndian()) {
			return mMemEndian == FFL_LITTLE_ENDIAN;
		}
		return mMemEndian == FFL_BIG_ENDIAN;
	}

	//
	//  order:顺序的，还是反序的
	//
	static inline void  internalCopyBytes(uint8_t* s, uint8_t* d, uint32_t size, bool order) {
		if (order) {
			memcpy(d, s, size);
		}
		else {
			for (uint32_t i = 0; i < size; i++) {
				d[size - i - 1] = s[i];
			}
		}
	}

	ByteStreamReader::ByteStreamReader():mReadPos(0) {
	}
	ByteStreamReader::~ByteStreamReader() {
	}
	//
	//  读写
	//
	bool ByteStreamReader::read1Bytes(int8_t& val) {
		uint32_t size = 1;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}

	bool ByteStreamReader::read2Bytes(int16_t& val) {
		uint32_t size = 2;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamReader::read3Bytes(int32_t& val) {
		uint32_t size = 3;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamReader::read4Bytes(int32_t& val) {
		uint32_t size = 4;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamReader::read8Bytes(int64_t& val) {
		uint32_t size = 8;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamReader::readString(String& val, uint32_t len)
	{
		uint32_t size = len;
		if (getSize() >= size) {
			uint8_t* buf = new uint8_t[size];
			readBuffer(buf, size, false);
			val.append((const char*)buf, size);
			delete[] buf;
			return true;
		}
		return false;
	}
	bool ByteStreamReader::readBytes(int8_t* val, uint32_t size) {		
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, true);
			return true;
		}
		return false;
	}
	void ByteStreamReader::readBuffer(uint8_t* dst, uint32_t size, bool order) {
		internalCopyBytes(getData() + mReadPos, dst, size, order);
		mReadPos += size;	
		mDataSize -= size;
	}


	ByteStreamWriter::ByteStreamWriter() :mWritePos(0){
	}
	ByteStreamWriter::~ByteStreamWriter() {
	}	

	bool ByteStreamWriter::write1Bytes(int8_t val) {
		uint32_t size = 1;
		if (getCapacity()-getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::write2Bytes(int16_t val) {
		uint32_t size = 2;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::write3Bytes(int32_t val) {
		uint32_t size = 3;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::write4Bytes(int32_t val) {
		uint32_t size = 4;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::write8Bytes(int64_t val) {
		uint32_t size = 8;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::writeString(const String& val, uint32_t len) {
		uint32_t size = len;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)val.c_str(), size, true);
			return true;
		}
		return false;
	}
	bool ByteStreamWriter::writeBytes(const int8_t* val, uint32_t size) {
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)val, size, true);
			return true;
		}
		return false;
	}
	void ByteStreamWriter::writeBuffer(uint8_t* src, uint32_t size, bool order) {
		internalCopyBytes(getData() + mWritePos,src, size, order);		
		mWritePos += size;
		mDataSize += size;
	}
	

	ByteStream::ByteStream():mReadPos(0),mWritePos(0){

	}
	ByteStream::~ByteStream() {
	}
	//
	//  重置读写指针
	//
	void ByteStream::reset(){
		mReadPos = 0;
		mWritePos = 0;
		mDataSize = 0;
	}
	//
	//  读写
	//
	bool ByteStream::read1Bytes(int8_t& val) {
		uint32_t size = 1;
		if (getSize()>=size) {
			readBuffer((uint8_t*)&val,size, isSameEndian());
			return true;
		}
		return false;
	}

	bool ByteStream::read2Bytes(int16_t& val) {
		uint32_t size = 2;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::read3Bytes(int32_t& val) {
		uint32_t size = 3;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::read4Bytes(int32_t& val) {
		uint32_t size = 4;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::read8Bytes(int64_t& val) {
		uint32_t size = 8;
		if (getSize() >= size) {
			readBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::readString(String& val, uint32_t len){
		uint32_t size = len;
		if (getSize() >= size) {
			uint8_t* buf = new uint8_t[size];
			readBuffer(buf, size, false);
			val.append((const char*)buf, size);
			delete[] buf;
			return true;
		}
		return false;
	}
	bool ByteStream::readBytes(int8_t* val, uint32_t size) {
		if (getSize() >= size) {
			readBuffer((uint8_t*)val, size, true);
			return true;
		}
		return false;
	}

	bool ByteStream::write1Bytes(int8_t val) {
		uint32_t size = 1;
		if (getCapacity()- getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::write2Bytes(int16_t val) {
		uint32_t size = 2;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::write3Bytes(int32_t val) {
		uint32_t size = 3;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::write4Bytes(int32_t val) {
		uint32_t size = 4;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::write8Bytes(int64_t val) {
		uint32_t size = 8;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)&val, size, isSameEndian());
			return true;
		}
		return false;
	}
	bool ByteStream::writeString(const String& val, uint32_t len) {
		uint32_t size = len;
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)val.c_str(), size, true);
			return true;
		}
		return false;
	}
	bool ByteStream::writeBytes(const int8_t* val, uint32_t size) {
		if (getCapacity() - getSize() >= size) {
			writeBuffer((uint8_t*)val, size, true);
			return true;
		}
		return false;
	}

	void ByteStream::readBuffer(uint8_t* dst, uint32_t size, bool reversal) {
		if (mReadPos + size <= getCapacity()) {
			if (dst) {
				internalCopyBytes(getData() + mReadPos, dst, size, reversal);
			}
			mReadPos += size;
		}else {
			uint8_t s1 = getCapacity() - mReadPos;
			if (dst) {
				internalCopyBytes(getData() + mReadPos, dst, s1, reversal);
				internalCopyBytes(getData(), dst + s1, size - s1, reversal);
			}
			mReadPos = size-s1;
		}
		mDataSize -= size;
	}
	void ByteStream::writeBuffer(uint8_t* src, uint32_t size, bool order) {
		if (mWritePos + size <= getCapacity()) {
			internalCopyBytes(src,getData() + mWritePos, size, order);
			mWritePos += size;			
		}
		else {
			uint8_t s1 = getCapacity() - mWritePos;
			internalCopyBytes(src,getData() + mWritePos,s1, order);
			internalCopyBytes(src +s1 ,getData(), size - s1, order);
			mWritePos = size - s1;
		}
		mDataSize += size;
	}
}
