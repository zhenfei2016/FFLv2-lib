/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetSocket.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  socket读写
*
*/

#include <net/base/FFL_Net.h>
#include <net/FFL_NetSocket.hpp>


namespace FFL {
	CSocket::CSocket(NetFD fd):mFd(fd){
	}
	CSocket::~CSocket() {
	}
	//
	//  读数据到缓冲区
	//  buf:缓冲区地址
	//  count:需要读的大小
	//  pReaded:实质上读了多少数据
	//  返回错误码  ： FFL_OK表示成功
	//
	status_t CSocket::read(uint8_t* buf, size_t count, size_t* pReaded) {
		return FFL_socketRead(mFd, buf, count, pReaded);
	}
	//
	//  写数据到文件中
	//  buf:缓冲区地址
	//  count:缓冲区大小
	//  pWrite:实质上写了多少数据
	//  返回错误码  ： FFL_OK表示成功
	//
	status_t CSocket::write(void* buf, size_t count, size_t* pWrite) {
		return FFL_socketWrite(mFd, buf, count, pWrite);
	}
	//
	//  写数据到文件中
	//  bufVec:缓冲区地址,数组
	//  count:数组大小
	//  pWrite:实质上写了多少数据
	//  返回错误码  ： FFL_OK表示成功
	//
	status_t CSocket::writeVec(const BufferVec* bufVec, int count, size_t* pWrite) {
		status_t ret = FFL_OK;
		size_t nWriteAll = 0;
		size_t nWrite;
		for (int32_t i = 0; i < count; i++) {
			const BufferVec* pBuf = bufVec + i;

			if (FFL_OK !=
				 (ret=FFL_socketWrite(mFd, pBuf->data, pBuf->size, &nWrite))
				){				
				break;
			}
			nWriteAll += nWrite;
		}
		
		if (pWrite) {
			*pWrite = nWriteAll;
		}
		return ret;
	}
}