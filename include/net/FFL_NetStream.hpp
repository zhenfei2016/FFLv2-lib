/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetStream.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/17 
*  https://github.com/zhenfei2016/FFL-v2.git
*  网络流，读取接口，内部缓存数据，方便进行parser
*
*/
#ifndef _FFL_NET_STREAM_READER_HPP_
#define _FFL_NET_STREAM_READER_HPP_

#include <utils/FFL_ByteReader.hpp>
#include <net/FFL_NetSocket.hpp>

namespace FFL {
	class ByteBuffer;

	class NetStreamReader : public ByteReader {
	public:
		NetStreamReader(CSocket* socket );
        virtual ~NetStreamReader();
	public:
		//
		//  获取数据指针
		//
		uint8_t* getData() const;
		//
		//  获取存储的数据大小
		//
		uint32_t getSize() const;
		//
		//  获取数据的开始为ZHi
		//
		uint32_t getPosition() const;
		//
		//  丢掉这么长度的数据
		//
		void skip(int32_t length);
		//
		//  socket拉取数据填充
		//
		status_t pull(int32_t expect);
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
	private:
		ByteBuffer* mBuffer;
		uint32_t mPosition;
		uint32_t mSize;

		CSocket* mSocket;
	};
}

#endif
