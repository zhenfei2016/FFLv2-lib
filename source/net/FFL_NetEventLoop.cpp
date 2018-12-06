/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetFdEvent.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/12/1
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  网路服务器基类
*
*/
#include <net/FFL_NetEventLoop.hpp>
#include <net/base/FFL_Net.h>

namespace FFL {
	const static int  FD_LIST_MAX_SIZE = 64;

	const static int8_t READABLE = 0x01;
	const static int8_t WRITABLE = 0x10;

	static int8_t CONTROL_COMMAND_A = 'a';

	NetEventLoop::NetEventLoop(int64_t evenloopWaitUs):mControlPort(0),
		 mControlFd(0),
		mWaitUs(evenloopWaitUs),
		mFdNum(0){
		mFdList = new FdEntry[FD_LIST_MAX_SIZE];
		memset(mFdList, 0,sizeof(mFdList[0])*FD_LIST_MAX_SIZE);
	}
	NetEventLoop::~NetEventLoop() {
		FFL_SafeFreeA(mFdList);

		if (mControlFd) {
			FFL_socketClose(mControlFd);
			mControlFd = 0;
		}
	}

	//
	// 添加一个监听的句柄， readHandler如果不等NULL，则在removeFd的时候会调用 readHandlerFree进行清理
	// 
	bool NetEventLoop::addFd(NetFD fd,
		NetEventHandler* readHandler,
		NetEventHandlerFree readHandlerFree,
		void* priv) {
		if (fd == NULL) {
			return false;
		}

		FFL::CMutex::Autolock l(mMutex);
		for (int32_t i = 0; i < FD_LIST_MAX_SIZE; i++) {
			FdEntry* entry = mFdList + i;
			if (fd == entry->mFd ) {
				if (entry->mRemoved) {
					entry->mFd = 0;
					mFdNum--;
					if (entry->mReadHandlerFree) {
						entry->mReadHandlerFree(entry->mReadHandler);
					}
				}
				else {
					return false;
				}
			}
			if (entry->mFd != NULL) {
				continue;
			}

			entry->mFd = fd;
			entry->mReadHandler = readHandler;
			entry->mReadHandlerFree = readHandlerFree;
			entry->mPriv = priv;

			entry->mRemoved = false;
			mFdNum++;
			mCond.signal();

			if (isStarted()&&mControlFd) {				
				FFL_socketWriteTo(mControlFd, &CONTROL_COMMAND_A, sizeof(CONTROL_COMMAND_A), NULL,"127.0.0.1" ,mControlPort );
			}
			return true;
		}

		return false;
	}
	void NetEventLoop::removeFd(NetFD fd) {
		FFL::CMutex::Autolock l(mMutex);
		NetEventLoop::FdEntry* entry = findFdEntry(fd);
		//
		// 标记一下删除
		//
		entry->mRemoved = true;
		mFdNum--;
	}
	//
	//   如果start使用了EventloopThread，则stop会阻塞到线程退出
	//   否则则仅仅置一下标志
	//
	bool NetEventLoop::onStart() {
		uint16_t port = 6100;
		while(port<6110){
			mControlFd = 0;
			mControlPort = port;
			if (FFL_OK == FFL_socketAnyAddrUdpServer(port++, &mControlFd)) {				
				break;
			}
		}

		if (mControlFd == 0) {
			return false;
		}		
		return true;
	}
	void NetEventLoop::onStop() {
		//
		//  触发退出select
		//

	
	}
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool NetEventLoop::eventLoop(int32_t* waitTime){		
		if (!isStarted()) {
			FFL_LOG_WARNING("Failed to NetEventLoop::eventLoop. not start.");
			return false;
		}
		//
		//  等待添加一个有效的句柄
		//
		while (mFdNum == 0) {
			mCond.wait(mMutex);
			if (!isStarted()) {
				return false;
			}
		}				


		NetFD fdList[FD_LIST_MAX_SIZE] = {};
		int numFd = 1;
		fdList[0] = mControlFd;
		for (int i = 0; i < FD_LIST_MAX_SIZE; i++) {
			if (mFdList[i].mRemoved || mFdList[i].mFd == NULL) {
				continue;
			}			
			fdList[numFd] = mFdList[i].mFd;
			numFd += 1;
		}


		if (numFd > 0) {
			int8_t flagList[FD_LIST_MAX_SIZE] = {};
			if (FFL_socketSelect(fdList, flagList, numFd, mWaitUs) > 0) {
				//
				// 派发可读消息
				//
				processReadableFd(fdList, flagList, numFd);
			}
			else {
			}
		}


		//
		//删除不需要的
		//
		processDidFd();
		return true;
	}


	//
	//  处理一下已经标记不用的fd
	//
	void NetEventLoop::processDidFd() {
		for (int i = 0; i < FD_LIST_MAX_SIZE; i++) {
			if (mFdList[i].mRemoved) {
				FFL_LOG_DEBUG(" remove fd=%d ", mFdList[i].mFd);
				mFdList[i].mFd = 0;
				removeFdEntryImpl(&mFdList[i]);
				memset(&mFdList[i], 0, sizeof(mFdList[0]));
				continue;
			}
		}
	}

	//
	//  处理一下可读的fd
	//
	void NetEventLoop::processReadableFd(NetFD* fdList, int8_t* readableFlagList,int32_t numFd) {
		if (readableFlagList[0]) {
			//
			//  本系统控制端口
			//
			mControlHandler.onNetEvent(mControlFd, true, false, false,  0);
			return;
		}


		FFL::CMutex::Autolock l(mMutex);
		for (int32_t i = 1; i < numFd; i++) {
			if (readableFlagList[i]==0) {
				continue;
			}

			NetEventLoop::FdEntry* entry = NULL;
			{
				FFL::CMutex::Autolock l(mMutex);
				entry=findFdEntry(fdList[i]);
			}

			if (entry->mRemoved) {
				continue;
			}

			//
			//  可以读了
			//
			if (entry->mReadHandler) {
				if (!entry->mReadHandler->onNetEvent(entry->mFd, true, false, false, entry->mPriv)) {
					//
					//  返回false，则不需要了
					//
					entry->mRemoved = true;
				}
			}
		}
	}
	
	NetEventLoop::FdEntry* NetEventLoop::findFdEntry(NetFD fd) {
		static NetEventLoop::FdEntry TmpFdEntry = {};
		for (int32_t i = 0; i < FD_LIST_MAX_SIZE; i++) {
			if (mFdList[i].mFd == fd) {
				return mFdList + i;
			}
		}

		TmpFdEntry.mRemoved = true;
		return &TmpFdEntry;
	}

	void NetEventLoop::removeFdEntryImpl(FdEntry* entry) {		
		if (entry->mReadHandlerFree) {
			entry->mReadHandlerFree(entry->mReadHandler);
		}
		entry->mRemoved = true;
	}

	
	//
	//  返回是否还继续读写
	//
	bool NetEventLoop::ConmtrolHandler::onNetEvent(NetFD fd, bool readable, bool writeable, bool exception,void* priv) {
		if (readable) {
			char buf[32] = {};
			size_t readed = 0;
			char srcIp[32] = {};
			uint16_t srcPort = 0;			
			FFL_socketReadFrom(fd, buf, 4096, &readed, srcIp, &srcPort);

			//if (CONTROL_COMMAND_A) {
			//}
		}
		return true;
	}
}
