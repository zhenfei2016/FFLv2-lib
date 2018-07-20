/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineAsyncConnector.cpp
*  Created by zhufeifei(34008081@qq.com) on 2017/12/10
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  node异步连接器,进行消息处理中消息的缓存异步处理
*
*/

#include <pipeline/FFL_PipelineAsyncConnector.hpp>
#include <pipeline/FFL_PipelineInput.hpp>
#include <pipeline/FFL_PipelineHandler.hpp>
#include <utils/FFL_Handler.hpp>
#include "PipelineLooper.hpp"


namespace FFL {

	class PipelineAsyncConnector::InputHandler : public PipelineHandler
	{
	public:
		InputHandler(PipelineAsyncConnector* conn) :mConn(conn)
		{}
		void handleMessage(const sp<PipelineMessage> &msg) {
			if (!msg.isEmpty()) {
				mConn->dispathMessage(msg);
			}
			else {
				FFL_LOG_WARNING("PipelineAsyncConnector::handleMessage msg is  null");
			}
		}
		//
		//  收到系统消息
		//
		void handleSysMessage(const sp<PipelineMessage> &msg) {
			if (!msg.isEmpty()) {
				mConn->dispathSysMessage(msg);
			}
			else {
				FFL_LOG_WARNING("PipelineAsyncConnector::handleMessage msg is  null");
			}
		}
		//
		//  退出消息循环
		//
		void handleQuitMessage(uint32_t exitCode)
		{
			mConn->dispathSysMessage(NULL);
		}
	private:
		PipelineAsyncConnector* mConn;
	};
	//
	//looper : 这个异步连接器在哪一个looper中跑
	//
	PipelineAsyncConnector::PipelineAsyncConnector() {
		mHandler = new InputHandler(this);
		mIsOuterLooper = false;
		mLooperClock = new Clock();
		mLooper = new PipelineLooper(mLooperClock);
		mFlags.resetFlags(READYING);
	}
	PipelineAsyncConnector::PipelineAsyncConnector(sp<PipelineLooper>  looper) {
		mHandler = new InputHandler(this);
		if (looper.isEmpty()) {
			mIsOuterLooper = false;
			mLooperClock = new Clock();
			mLooper = new PipelineLooper(NULL);
		}
		else {
			mIsOuterLooper = true;
			mLooper = looper;
		}

		mFlags.resetFlags(READYING);
	}
	PipelineAsyncConnector::~PipelineAsyncConnector() {
	}

	//
	//  启动，停止这个连接器
	//
	status_t PipelineAsyncConnector::startup() {
		if (!isConnected()) {
			FFL_LOG_WARNING("PipelineAsyncConnector startup fail  not connect");
			return FFL_ERROR_FAIL;
		}

		mFlags.resetFlags(STARTING);
		registerHandler();
		if (mIsOuterLooper) {
			return FFL_OK;
		}

		String name;
		getInput()->getName(name);
		mLooper->setName(name.c_str());
		FFL_LOG_DEBUG("looper (%s) starting", name.c_str());
		status_t ret = mLooper->start();
		FFL_LOG_DEBUG("looper (%s) started %s", name.c_str(), (ret == FFL_OK ? "ok" : "faild"));
		return ret;
	}
	status_t PipelineAsyncConnector::shutdown() {
		requestShutdown();
		return waitShutdown();
	}

	//
	//  请求shutdown,等待shutdown结束
	//
	status_t PipelineAsyncConnector::requestShutdown() {
		mFlags.resetFlags(SHUTDOWNING);
		unregisterHandler();
		if (mIsOuterLooper) {
			return FFL_OK;
		}

		status_t status = mLooper->requestStop();
		return status;
	}
	status_t PipelineAsyncConnector::waitShutdown() {
		if (mIsOuterLooper) {
			return FFL_OK;
		}

		mLooper->waitStop();
		mLooper->clearMessage();
		return FFL_OK;
	}

	sp<PipelineLooper> PipelineAsyncConnector::getLooper() {
		return mLooper;
	}

	//
	// 连接器转发消息
	//
	status_t PipelineAsyncConnector::tranport(const sp<PipelineMessage> &msg, int64_t delayUs)
	{
		return postMessage(msg, delayUs);
	}
	//
	//  获取时钟，这个是tranport使用的，delayUs会通过这个时钟进行计算延迟多长时间
	//
	sp<Clock> PipelineAsyncConnector::getClock() {
		return mLooperClock;
	}
	void PipelineAsyncConnector::setDebug(bool debug) {
		getLooper()->setDebug(debug);
	}
	//
	// 消息异步发送
	//
	status_t PipelineAsyncConnector::postMessage(const sp<PipelineMessage> &msg, int64_t delayUs)
	{
		sp<PipelineInput> input = getInput();
		if (input.isEmpty()) {
			FFL_LOG_WARNING("SyncPipelineConnector input is null");
			return FFL_ERROR_FAIL;
		}
		else {
			//
			//  等待looper启动起来，这个需要注意，可能会卡线程好长一段时间
			//
			if (!mLooper->isLooping()) {
				FFL_LOG_WARNING("PipelineAsyncConnector wait looping...");
				if (mLooper->waitLooping(-1)) {

				}
				FFL_LOG_WARNING("PipelineAsyncConnector looping");
			}

			if (mHandler->id() == 0) {
				FFL_LOG_WARNING("PipelineAsyncConnector handlerId=0");
			}
			//
			// 发送
			//
			if (!mLooper->postMessage(msg, mHandler->id(), delayUs)) {


			}
		}
		return FFL_OK;
	}
	//
	//  清空转发的消息
	//
	void PipelineAsyncConnector::clearMessage() {
		FFL_LOG_DEBUG("PipelineAsyncConnector::clearMessage %s",getName());
		if (!mLooper.isEmpty() && !mHandler.isEmpty()) {
			mLooper->clearMessage(mHandler->id());
		}
	}
	//
	// 开始分派消息
	//
	void PipelineAsyncConnector::dispathMessage(const sp<PipelineMessage>& msg) {
		sp<PipelineInput> input = getInput();
		if (!input.isEmpty()) {
			input->dispathMessage(msg);
		}
	}

	//
	//  注册/反注册处理句柄到looper中
	//
	void PipelineAsyncConnector::registerHandler() {
		getLooper()->registerHandler(mHandler);
	}
	void PipelineAsyncConnector::unregisterHandler() {
		if (!mHandler.isEmpty()) {
			Looper::handler_id id = mHandler->id();
			getLooper()->clearMessageList(NULL, id);
			getLooper()->unregisterHandler(id);
		}
	}

	//
	//  是否已经停止运行了
	//
	bool PipelineAsyncConnector::isShutdowned() const
	{
		return mFlags.hasFlags(SHUTDOWNING);

	}
}
