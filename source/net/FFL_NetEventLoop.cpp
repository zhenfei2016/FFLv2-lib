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
#include "FFL_SocketPair.hpp"

namespace FFL {
	const static int  FD_LIST_MAX_SIZE = 64;

	const static int8_t READABLE = 0x01;
	const static int8_t WRITABLE = 0x10;

	class EventPacket {
	public:
		int64_t mEventIndex;
		//
		//  添加还是删除指令
		//
		bool mCommandAdd;
		bool mCommandRemove;
		//
		//  传输的参数
		//
		NetFD fd;
		NetEventHandler* readHandler;
		NetEventHandlerFree readHandlerFree;
		void* priv;
	};


	NetEventLoop::NetEventLoop(int64_t evenloopWaitUs):
		mControlFd(0),
		mOnlyTryControlFd(false),
		mWaitUs(evenloopWaitUs),
		mSocketPairControl(NULL),
		mFdNum(0),
		mStopLoop(false){
		mEventNextId = 0;
		mFdList = new FdEntry[FD_LIST_MAX_SIZE];
		memset(mFdList, 0,sizeof(mFdList[0])*FD_LIST_MAX_SIZE);
	}

	NetEventLoop::~NetEventLoop() {
		FFL_SafeFreeA(mFdList);
		FFL_SafeFree(mSocketPairControl);
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
		if (fd == INVALID_NetFD || readHandler ==NULL ) {
			return false;
		}	

		if (mControlFd == INVALID_NetFD) {
			//
			// 还没有启动的情况下
			//
			return  processAddFd(fd, readHandler, readHandlerFree, priv);

		}

		EventPacket* packet = new EventPacket();
		packet->mEventIndex=mEventNextId++;
		packet->mCommandAdd = true;
		packet->mCommandRemove = false;
		packet->fd = fd;
		packet->readHandler = readHandler;
		packet->readHandlerFree = readHandlerFree;
		packet->priv = priv;
		if (!mSocketPairControl->writeFd0((uint8_t*)(&packet), sizeof(packet), 0)) {
			FFL_SafeFree(packet);
			return false;
		}

		addEvent(packet);
		return true;
	}
	bool NetEventLoop::removeFd(NetFD fd) {
		if (fd == INVALID_NetFD) {
			return false;
		}
		if (mControlFd == INVALID_NetFD) {
			NetEventLoop::FdEntry* entry = findFdEntry(fd);
			return processRemoveFd(entry);
		}
		EventPacket* packet = new EventPacket();
		packet->mEventIndex = mEventNextId++;
		packet->mCommandAdd = false;
		packet->mCommandRemove = true;
		packet->fd = fd;
		packet->readHandler = NULL;
		packet->readHandlerFree = NULL;
		packet->priv = NULL;
		if (!mSocketPairControl->writeFd0((uint8_t*)(&packet), sizeof(packet), 0)) {
			FFL_SafeFree(packet);
			return false;
		}
		
		addEvent(packet);
		return true;
	}
	//
	//   如果start使用了EventloopThread，则stop会阻塞到线程退出
	//   否则则仅仅置一下标志
	//
	bool NetEventLoop::onStart() {	
		mStopLoop = false;
		FFL_SafeFree(mSocketPairControl);
		mSocketPairControl = new SocketPair();
		if (!mSocketPairControl->create()) {
			FFL_SafeFree(mSocketPairControl);
			return false;
		}	

		mControlFd = mSocketPairControl->getFd1();
		return true;
	}
	void NetEventLoop::onStop() {
		//
		//  触发退出select
		//
		mStopLoop = true;
		if (mSocketPairControl) {
			uint8_t c = 'x';
			mSocketPairControl->writeFd0(&c, 1,0);
			mSocketPairControl->destroy();
		}	
	}
	//
	//   阻塞的线程中执行的eventloop,返回是否继续进行eventLoop
	//   waitTime:输出参数，下一次执行eventLoop等待的时长
	//   true  : 继续进行下一次的eventLoop
	//   false : 不需要继续执行eventloop
	//
	bool NetEventLoop::eventLoop(int32_t* waitTime){		
		if (!isStarted()) {
			FFL_LOG_WARNING("NetEventLoop: Failed to NetEventLoop::eventLoop. not start.");
			return false;
		}		

		
		NetFD fdList[FD_LIST_MAX_SIZE] = {};
		int numFd = 1;
		fdList[0] = mControlFd;

		//
		//  外部add的
		//
		if (!mOnlyTryControlFd) {
			for (int i = 0; i < FD_LIST_MAX_SIZE && numFd <= mFdNum; i++) {
				if (mFdList[i].mRemoved || mFdList[i].mFd == NULL) {
					continue;
				}
				fdList[numFd] = mFdList[i].mFd;
				numFd += 1;
			}
		}
				
		FFL_LOG_DEBUG("NetEventLoop: select fdNum=%d mOnlyTryControlFd=%d",numFd, mOnlyTryControlFd?1:0);
		int8_t flagList[FD_LIST_MAX_SIZE] = {};
		if (FFL_socketSelect(fdList, flagList, numFd, mWaitUs) > 0) {
			//
			// 派发可读消息
			//
			if (!processReadableFd(fdList, flagList, numFd)) {
				return false;
			}
			mOnlyTryControlFd = false;
		}
		else {
			//
			//  不使用锁，没问题
			//
			if (mStopLoop) {
				return false;
			}

			{   //
				//  处理一下可能没有发送过来的消息
				//
				CMutex::Autolock l(mEventsLock);
				for (List<EventPacket*>::iterator it = mPendingEvents.begin(); it != mPendingEvents.end(); ) {
					EventPacket* packet = (*it);
					if (packet) {
						if (packet->mCommandAdd) {
							processAddFd(packet->fd, packet->readHandler, packet->readHandlerFree, packet->priv);
						}
						if (packet->mCommandRemove) {
							NetEventLoop::FdEntry* entry = findFdEntry(packet->fd);
							processRemoveFd(entry);
						}
					}
					it = mPendingEvents.erase(it);
				}
			}

			FFL_LOG_DEBUG("NetEventLoop: select failed");			
			mOnlyTryControlFd = true;
			FFL_sleep(100);
		}	

		return true;
	}


	bool NetEventLoop::processAddFd(NetFD fd,
		NetEventHandler* readHandler,
		NetEventHandlerFree readHandlerFree,
		void* priv) {

		for (int32_t i = 0; i < FD_LIST_MAX_SIZE; i++) {
			FdEntry* entry = mFdList + i;
			if (fd == entry->mFd) {
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
			FFL_LOG_DEBUG("NetEventLoop: add fdNum=%d",mFdNum+1);
			break;
		}
		return true;
	}
	//
	//  移除这个句柄的处理handler
	//
	bool NetEventLoop::processRemoveFd(NetEventLoop::FdEntry* entry) {		
		if (!entry) {
			return false;
		}

		if (entry->mReadHandlerFree) {
			entry->mReadHandlerFree(entry->mReadHandler);
		}
		memset(entry, 0, sizeof(NetEventLoop::FdEntry));
		mFdNum--;
		FFL_LOG_DEBUG("NetEventLoop: remove fdNum=%d", mFdNum+1);
		return true;
	}

	//
	//  处理一下可读的fd
	//
	bool NetEventLoop::processReadableFd(NetFD* fdList, int8_t* readableFlagList,int32_t numFd) {
		if (readableFlagList[0]) {
			//
			//  本系统控制端口
			//
			return processControlEvent(mControlFd, true, false, false,  0);			
		}
		
		for (int32_t i = 1; i < numFd; i++) {
			if (readableFlagList[i]==0) {
				continue;
			}

			NetEventLoop::FdEntry* entry = findFdEntry(fdList[i]);
			if (entry == NULL) {
				continue;
			}

			if (!entry->mRemoved) {
				//
				//  可以读了
				//				
				FFL_LOG_DEBUG("NetEventLoop: onNetEvent  fd=%d", entry->mFd);
				if (!entry->mReadHandler->onNetEvent(entry->mFd, true, false, false, entry->mPriv)) {
					//
					//  返回false，则不需要了
					//
					entry->mRemoved = true;
				}
			}

			if (entry->mRemoved) {
				FFL_LOG_DEBUG("NetEventLoop: onNetEvent return false. fd=%d", entry->mFd);
				processRemoveFd(entry);
			}
		}

		return true;
	}
	
	NetEventLoop::FdEntry* NetEventLoop::findFdEntry(NetFD fd) {		
		for (int32_t i = 0; i < FD_LIST_MAX_SIZE; i++) {
			if (mFdList[i].mFd == fd) {
				return mFdList + i;
			}
		}
		return NULL;
	}

	
	//
	//  返回是否还继续读写
	//
	bool NetEventLoop::processControlEvent(NetFD fd, bool readable, bool writeable, bool exception,void* priv) {
		if (!readable) {
			return true;
		}

		EventPacket* packet = NULL;
		size_t readed = 0;
		if (FFL_OK != FFL_socketRead(fd, &packet, sizeof(packet), &readed)) {
			readed = 1;
		}
		if (readed == 1) {	
			//
			//  退出系统
			//
			FFL_LOG_DEBUG("NetEventLoop: processControlEvent quit");
			return false;
		}
		if (readed != sizeof(packet) || packet ==NULL ) {
			return true;
		}

		if (packet->mCommandAdd) {		
			processAddFd(packet->fd, packet->readHandler, packet->readHandlerFree, packet->priv);
		}
		if (packet->mCommandRemove) {
			NetEventLoop::FdEntry* entry = findFdEntry(fd);
			processRemoveFd(entry);
		}

		removeEvent(packet);
		FFL_SafeFree(packet);
		return true;
	}

	void NetEventLoop::addEvent(EventPacket* event) {
		CMutex::Autolock l(mEventsLock);
		mPendingEvents.push_back(event);
	}
	void NetEventLoop::removeEvent(EventPacket* event) {
		CMutex::Autolock l(mEventsLock);
		for (List<EventPacket*>::iterator it = mPendingEvents.begin(); it != mPendingEvents.end(); ) {
			if (*it == event) {
				it=mPendingEvents.erase(it);
			}
			else {
				it++;
			}
		}
	}

}
