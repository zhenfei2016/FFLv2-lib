/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HandlerManager.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*  https://github.com/zhenfei2016/FFL-v2.git
*  handler管理器，每一个looper中存在一个这个示例，用于handler的管理
*
*/

#include <utils/FFL_HandlerManager.hpp>
#include <utils/FFL_Message.hpp>

namespace FFL {

	static uint32_t gNextHandlerID=1;	
	static void generateNextHandlerID(Looper::handler_id& id) {
		id = gNextHandlerID++;
	}

	HandlerManager::HandlerManager(Looper* looper):mLooper(looper)
	{}
	HandlerManager::~HandlerManager()
	{}
	//
	//  关联上handler和looper，并注册到系统中。
	//  注册成功后，对应hander_id的消息会得到这个handler的处理
	//  返回hanler_id
	//
	Looper::handler_id HandlerManager::addHandler(const sp<Handler> &handler)
	{
		CMutex::Autolock l(mLock);

		if (handler->id() != 0) {
			FFL_LOG_WARNING("A handler must only be registered once.");
			return FFL_INVALID_OPERATION;
		}

		HandlerInfo info;		
		info.mHandler = handler;
		Looper::handler_id handlerID;
		generateNextHandlerID(handlerID);
		mHandlers[handlerID] = info;
		handler->setID(handlerID);

		return handlerID;
	}

	//
	// 取消handlerID的handler
	//
	void HandlerManager::removeHandler(Looper::handler_id handlerID)
	{
		CMutex::Autolock l(mLock);		

		KeyedVectorHandler::iterator index = mHandlers.find(handlerID);
		if (index == mHandlers.end()) {
			return;
		}

		const HandlerInfo &info = index->second;
		sp<Handler> handler = info.mHandler.promote();
		if (handler != NULL) {
			handler->setID(0);
		}

		mHandlers.erase(index);
	}

	//
	// 取消一些无效的handler,可能外面已经把这个handler删除掉了
	//
	void HandlerManager::removeStaleHandlers()
	{
		CMutex::Autolock l(mLock);

		std::list<Looper::handler_id> invalidHandler;
		KeyedVectorHandler::iterator i = mHandlers.begin();
		for (; i != mHandlers.end(); i++) {
			const HandlerInfo &info = i->second;
			sp<Handler> handler = info.mHandler.promote();
			if (handler == NULL) {
				FFL_LOG_INFO("Unregistering stale handler %d", i->first);
				invalidHandler.push_back(i->first);
			}
		}

		for (std::list<Looper::handler_id> ::iterator it = invalidHandler.begin();
			it != invalidHandler.end(); it++) {
			mHandlers.erase(*it);
		}
	}

	void HandlerManager::removeAllHandler(){
		CMutex::Autolock l(mLock);		
		KeyedVectorHandler::iterator i = mHandlers.begin();
		for (; i != mHandlers.end(); i++) {
			const HandlerInfo &info = i->second;
			sp<Handler> handler = info.mHandler.promote();
			if (handler != NULL) {
				handler->setID(0);
			}
		}
		mHandlers.clear();

	}

	//
	//  获取处理句柄
	//
	sp<FFL::Handler> HandlerManager::getHandler(Looper::handler_id handlerID) {
		CMutex::Autolock l(mLock);

		KeyedVectorHandler::iterator index = mHandlers.find(handlerID);
		if (index == mHandlers.end()) {
			FFL_LOG_WARNING("failed to deliver message. Target handler(%d) not registered.", handlerID);
			return NULL;
		}

		const HandlerInfo &info = index->second;
		sp<FFL::Handler> handler = info.mHandler.promote();

		if (handler == NULL) {		
			mHandlers.erase(index);
			return NULL;
		}
		return handler;
	}
	//
	// 获取所有的handler列表
	//
	void HandlerManager::getAllHandler(List< sp<FFL::Handler> >& list)
	{
		CMutex::Autolock l(mLock);

		KeyedVectorHandler::iterator index = mHandlers.begin();
		for (; index != mHandlers.end();index++) {
			const HandlerInfo &info = index->second;
			sp<FFL::Handler> handler = info.mHandler.promote();
			if (!handler.isEmpty()) {
				list.push_back(handler);
			}
		}
	}
	//
	//进行这个消息的处理，查找对应消息的handler进行处理
	//
	void HandlerManager::deliverMessage(const sp<Message> &msg)
	{
		sp<FFL::Handler> handler = getHandler(msg->target());
		if (!handler.isEmpty()) 
		{
			handler->onMessageReceived(msg);
		}
	}
	//
	// 把消息投递给所有的handler
	//
	void HandlerManager::deliverMessageToAllHandler(const sp<Message> &msg)
	{

	}
}