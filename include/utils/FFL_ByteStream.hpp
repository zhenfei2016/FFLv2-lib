/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_ByteStream   
*  Created by zhufeifei(34008081@qq.com) on 2018/05/1
*  https://github.com/zhenfei2016/FFL-v2.git
*  字节流操作，读写
*/
#ifndef _FFL_BYTE_STREAM_HPP_
#define _FFL_BYTE_STREAM_HPP_

#include <FFL.h>
#include "FFL_ByteReader.hpp"
#include "FFL_ByteWriter.hpp"

namespace FFL{ 
	class ByteStreamBase {
	public:
		ByteStreamBase();
		virtual ~ByteStreamBase();
		//
		//   原始指针，指针指向的缓冲容量
		//
		uint8_t* getData() const { return mData; }
		//
		//  缓存容量
		//
		uint32_t getCapacity() const { return mDataCapacity; }
		//
		//  缓存中当前的有效数据
		//
		uint32_t getSize() const { return mDataSize; }
	public:
		//
		//  设置数据，bytestream将在这个数据集上进行操作
		//  data :缓冲起始位置
		//  size: 缓冲中当前有效数据大小
		//  capacity:缓冲容量
		//  memEndian:默认本系统字节序
		//
		virtual void setData(uint8_t* data, uint32_t size, uint32_t capacity);
		virtual void setData(uint8_t* data, uint32_t size, uint32_t capacity, int memEndian);
	protected:
		//
		//  是否buffer指向的数据跟系统的字节序一致
		//
		bool isSameEndian();
	protected:	
		//
		//  数据内存
		//
		uint8_t* mData;		
		uint32_t mDataCapacity;
		uint32_t mDataSize;
		//
		//  当前mData存储的数据字节序
		//
		uint32_t mMemEndian;
	};

	class ByteStreamReader :public ByteStreamBase, public ByteReader {
	public:
		ByteStreamReader();
		~ByteStreamReader();
	public:
		//
		//  ByteReader 读写
		//
		bool read1Bytes(int8_t& val);
		bool read2Bytes(int16_t& val);
		bool read3Bytes(int32_t& val);
		bool read4Bytes(int32_t& val);
		bool read8Bytes(int64_t& val);
		bool readString(String& val, uint32_t len);
		bool readBytes(int8_t* data, uint32_t size);
	protected:
		void readBuffer(uint8_t* dst, uint32_t size, bool order);
	private:
		uint32_t mReadPos;
	};


	class ByteStreamWriter :public ByteStreamBase,public ByteWriter{
	public:
		ByteStreamWriter();
		~ByteStreamWriter();		
	public:
		//
		//  ByteWriter
		//
		bool write1Bytes(int8_t val);
		bool write2Bytes(int16_t val);
		bool write3Bytes(int32_t val);
		bool write4Bytes(int32_t val);
		bool write8Bytes(int64_t val);
		bool writeString(const String& val, uint32_t len);
		bool writeBytes(const int8_t* data, uint32_t size);
	private:
		void writeBuffer(uint8_t* src, uint32_t size, bool order);
	private:
		//
		//  写位置
		//		
		uint32_t mWritePos;
	};


	

	//
	//  环形缓冲字节流
	//
	class ByteStream :public ByteStreamBase ,public ByteWriter,public ByteReader {
	public:
		ByteStream();
		~ByteStream();
		//
		//  重置读写指针
		//
		void reset();
		//
		//  ByteReader 读
		//
		bool read1Bytes(int8_t& val);
		bool read2Bytes(int16_t& val);
		bool read3Bytes(int32_t& val);
		bool read4Bytes(int32_t& val);
		bool read8Bytes(int64_t& val);
		bool readString(String& val, uint32_t len);
		bool readBytes(int8_t* val, uint32_t size);
		//
		//  ByteWriter 写
		//
		bool write1Bytes(int8_t val);
		bool write2Bytes(int16_t val);
		bool write3Bytes(int32_t val);
		bool write4Bytes(int32_t val);
		bool write8Bytes(int64_t val);
		bool writeString(const String& val, uint32_t len);
		bool writeBytes(const int8_t* val, uint32_t size);
	protected:
		//
		//  读写指定的字节
		//
		void readBuffer(uint8_t* dst, uint32_t size, bool reversal);
		void writeBuffer(uint8_t* dst, uint32_t size, bool reversal);
	private:
		//
		//  读写位置
		//
		uint32_t mReadPos;
		uint32_t mWritePos;	
	};
}



#endif

