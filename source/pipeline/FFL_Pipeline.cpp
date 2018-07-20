/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_Pipeline.cpp 
 *  Created by zhufeifei(34008081@qq.com) on 2017/12/02 
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *
 *  流水线实例，可以包含n多的节点 ，然后启动，结束流水线
 *
*/
#include <pipeline/FFL_Pipeline.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineEventCallback.hpp>
#include "FFL_PipelineNodeManager.hpp"
#include <pipeline/FFL_PipelineConnector.hpp>

namespace FFL{
	Pipeline::Pipeline(const char* scrip):mTrackStorage(NULL)
	{
		FFL_LOG_DEBUG("Pipeline::Pipeline %p", this);
		mNodeManager = new PipelineNodeManager(this);		
		mEventStartup = new PipelineEvent(new ClassMethodCallback<Pipeline>(this, &Pipeline::handleStartup));
		mEventStartupPending = false;

		mEventShutdown = new PipelineEvent(new ClassMethodCallback<Pipeline>(this, &Pipeline::handleShutdown));
		mEventShutdownPending = false;
		mEventFilter = NULL;		
		
	}	
	Pipeline::~Pipeline()
	{	
		if (mFlags.getFlags() != 0) {
			FFL_LOG_WARNING("delete Pipeline flag!=0  (%d)",mFlags.getFlags());
		}
	
		if (mNodeManager) {			
			delete mNodeManager;
			mNodeManager = NULL;
		}	
		FFL_LOG_DEBUG("Pipeline::~Pipeline %p", this);
	}
	void Pipeline::onFirstRef() {
        sp<Pipeline> pipeline=this;
		mEventLooper = new PipelineEventLooper(pipeline);
		mEventLooper->setName("pipelineEventLooper");
		mEventLooper->start();
	}

	void Pipeline::onLastStrongRef(const void* id) {
		if (!mEventLooper.isEmpty()) {
			mEventLooper->stop();
			mEventLooper.clear();
		}
	}
	//
	//  启动pipeline, 需要配置完PipelineNodeManager
	//  异步的
	//  成功返回 FFL_OK
	//
	status_t Pipeline::startup()
	{
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || mFlags.getFlags() !=0 ) {
			return FFL_INVALID_OPERATION;
		}

		mEventStartupPending = true;
		postEvent(mEventStartup);
		return FFL_OK;
	}
	//
	//  单独启动一个node，只有在整个pipeline启动后才能单独启动
	//  异步的
	//  成功返回 FFL_OK		
	//
	class SingleNodeStartupEvent : public PipelineEvent {
	public:
		SingleNodeStartupEvent(Pipeline* p, PipelineNodeId nodeId):
			PipelineEvent(new ClassMethodCallback<Pipeline>(p, &Pipeline::handleSingleNodeStartup))			
		{
			mNodeId[0] = nodeId;
			mCount = 1;
		}

		SingleNodeStartupEvent(Pipeline* p, PipelineNodeId* nodeId,int count) :
			PipelineEvent(new ClassMethodCallback<Pipeline>(p, &Pipeline::handleSingleNodeStartup))
		{
			int maxCount = FFL_MAX(count, 10);
			for (int32_t i = 0; i < maxCount; i++) {
				mNodeId[i] = nodeId[i];
			}
			mCount = maxCount;
		}

		PipelineNodeId mNodeId[10];
		int32_t mCount;
	};
	//
	//  注意只有在整个pipeline启动后才能调用这函数
	//  同步，异步启动所有的node，	如果node已经启动则会忽略
	//  如果未启动则启动它
	//
	status_t Pipeline::ansyStartupAllNode() {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}

		EventCallback* cb= new ClassMethodCallback<Pipeline>(this, &Pipeline::handleAllNodeStartup);
		postEvent(new PipelineEvent(cb));
		return FFL_OK;
	}
	status_t Pipeline::syncStartupAllNode() {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}

		handleAllNodeStartup(NULL);
		return FFL_OK;
	}
	//
	// 同步，异步启动启动一个node，
	// 注意只有在整个pipeline启动后才能单独启动
	//  成功返回 FFL_OK		
	//
	status_t Pipeline::syncStartupNode(PipelineNodeId nodeId) {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}
		handleSingleNodeStartup(new SingleNodeStartupEvent(this, nodeId));
		return FFL_OK;
	}
	status_t Pipeline::ansyStartupNode(PipelineNodeId nodeId) {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}
		postEvent(new SingleNodeStartupEvent(this, nodeId));
		return FFL_OK;
	}
	status_t Pipeline::syncStartupNode(PipelineNodeId* nodeId, int32_t count) {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}
		for (int32_t i = 0; i < count; i++) {
			handleSingleNodeStartup(new SingleNodeStartupEvent(this, nodeId[i]));
		}
		return FFL_OK;
	}
	status_t Pipeline::ansyStartupNode(PipelineNodeId* nodeId, int32_t count) {
		CMutex::Autolock l(mEventMutex);
		if (mEventStartupPending || (mFlags.hasFlags(SHUDOWNING))) {
			return FFL_INVALID_OPERATION;
		}
		postEvent(new SingleNodeStartupEvent(this, nodeId,count));
		return FFL_OK;
	}
	//
	//  停止pipeline,异步的
	//
	status_t Pipeline::shutdown(){
		CMutex::Autolock l(mEventMutex);
		if (mEventShutdownPending ||isLooping()==false ) {
			return FFL_INVALID_OPERATION;
		}

		mEventShutdownPending = true;
		postEvent(mEventShutdown);
		return FFL_OK;
	}
	//
	//  停止pipeline,并等待成功
	//
	status_t Pipeline::shutdownAndWait()
	{
		status_t status=shutdown();
		if(status==FFL_OK)
		{
			sp<Pipeline> pipeline = this;
			CMutex::Autolock l(mShutdownMutex);
			mShutdownCond.wait(mShutdownMutex);
		}
		return status;
	}
	//
	//  调用这个以后这个pipeline 就不能使用了
	//
	void Pipeline::exit() {
		shutdownAndWait();
		mEventLooper->stop();
		mEventLooper.clear();
	}
	//
	//  启动的处理逻辑，这个在事件线程中执行
	//
	void Pipeline::handleStartup(const sp<PipelineEvent>& event)
	{
		mFlags.modifyFlags(STARTUPING,0);
		uint32_t count = mNodeManager->getCount();
		for (uint32_t i = 0; i < count; i++) {			
			PipelineNode* node=mNodeManager->getNode(i);
			node->startup();
		}
		mFlags.modifyFlags(LOOPING,STARTUPING);
		{
			CMutex::Autolock l(mEventMutex);
			mEventStartupPending = false;
		}
	}
	//
	//  单个node启动的处理逻辑，
	//
	void Pipeline::handleSingleNodeStartup(const sp<PipelineEvent>& event){
		{
			CMutex::Autolock l(mEventMutex);
			if (mFlags.hasFlags(SHUDOWNING)) {			
				return;
			}
		}

		PipelineNodeId nodeId = Pipeline_INVALID_Id;
		SingleNodeStartupEvent* evt = (SingleNodeStartupEvent*)(event.get());
		if (evt != NULL) {

			for (int32_t i = 0; i < evt->mCount; i++) {
				nodeId = evt->mNodeId[i];
				//
				//  启动单个节点
				//
				if (nodeId != Pipeline_INVALID_Id) {
					PipelineNode* node = mNodeManager->findNode(nodeId);
					if (node)
						node->startup();
				}
			}
		}
	}
	//
	//  ansyStartupAllNode 触发的所有node进行启动
	//
	void Pipeline::handleAllNodeStartup(const sp<PipelineEvent>& event){
		{
			CMutex::Autolock l(mEventMutex);
			if (mFlags.hasFlags(SHUDOWNING)) 
				return;			
		}

		for (uint32_t index = 0; index < mNodeManager->getCount(); index++) {
			PipelineNode* node = mNodeManager->getNode(index);
			if (node)
				node->startup();
		}
	}
	//
	//  停止的处理逻辑，这个在事件线程中执行
	//
	void Pipeline::handleShutdown(const sp<PipelineEvent>& event)
	{		
		mFlags.modifyFlags(SHUDOWNING, LOOPING);
		uint32_t count = mNodeManager->getCount();
		for (uint32_t i = 0; i < count; i++) {
			PipelineNode* node = mNodeManager->getNode(i);
			node->shutdown();
		}
		mFlags.resetFlags(0);

		{
			CMutex::Autolock l(mEventMutex);
			mEventShutdownPending = false;
		}

		mShutdownCond.signal();
	}
	//
	// 发送事件到事件系统中， 异步的
	//
	status_t Pipeline::postEvent(const sp<PipelineEvent>& msg)
	{
		if (!msg.isEmpty()) 
		{			
			mEventLooper->postEvent(msg, 0);
		}
		return FFL_OK;
	}
	//
	// 设置事件过滤器
	//
	void Pipeline::setEventFilter(PipelineEventFilter* filter)
	{
		mEventFilter = filter;
	}

	void Pipeline::onEvent(const sp<PipelineEvent> &event)
	{		
		if (mEventFilter && mEventFilter->onPrepareDispatch(event)==false) {
			return;
		}

		for (uint32_t i = 0; i < getNodeManager()->getCount(); i++) {
			PipelineNode* node = getNodeManager()->getNode(i);
			if (node) {
				node->onEvent(event);
			}
		}
	}
	//
	// 获取节点管理器，返回这个Pipeline持有的PipelineNode
	// 生命周期与这个Pipeline一致的
	//
	PipelineNodeManager*  Pipeline::getNodeManager() {
		return mNodeManager;
	}

	//
	// 创建一个Node节点
	// handler： 为节点的处理逻辑句柄
	// 返回Node节点的 id  , PipelineNode::INVALID_NodeId 表示无效值
	//
	PipelineNodeId  Pipeline::createNode(sp<PipelineNodeHandler> handler) {
		return getNodeManager()->createNode(handler);		
	}
	//
	// 创建一个输入接口
	//
	PipelineInputId Pipeline::createInput(PipelineNodeId nodeId, sp<PipelineInputHandler> handler, const char* name)
	{
		PipelineNode* node=getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("createInput fail . not find node (%d)", nodeId);
			return Pipeline_INVALID_Id;
		}
		return node->createInput(name,handler);
	}
	//
	// 删除一个输入接口
	//
	void Pipeline::destroyInput(PipelineNodeId nodeId, PipelineInputId id)
	{
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("destroyInput fail . not find node (%d)", nodeId);
			return;
		}
		node->destroyInput(id);		
	}
	//
	// 创建一个输出接口
	//
	PipelineOutputId Pipeline::createOutput(PipelineNodeId nodeId){
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("createOutput fail . not find node (%d)", nodeId);
			return Pipeline_INVALID_Id;
		}
		return node->createOutput();
	}
	//
	// 删除一个输出接口
	//
	void Pipeline::destroyOutput(PipelineNodeId nodeId, PipelineOutputId id)
	{
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("destroyOutput fail . not find node (%d)", nodeId);
			return;
		}
		node->destroyOutput(id);
	}
	//
	//删除一个Node节点 ，节点删除时候，所有的输入，输出接口都断掉
	//
	void Pipeline::destroyNode(PipelineNodeId nodeId) {
		getNodeManager()->destroyNode(nodeId);
	}
	//
	//连接一个Node的输出口到另一个的输入口
	//conn :连接类型
	//
	status_t Pipeline::connect(
		PipelineNodeId srcNodeId, PipelineOutputId srcOutputId,
		PipelineNodeId dstNodeId, PipelineInputId dstInputId,
		sp<PipelineConnector> conn) {
		if (conn.is_empty()) {
			return FFL_INVALID_PARAMS;
		}

		PipelineNode* srcNode =getNodeManager()->findNode(srcNodeId);
		if (!srcNode) {
			FFL_LOG_WARNING("Pipeline::connect fail . not find node (%d)", srcNodeId);
			return FFL_INVALID_PARAMS;
		}

		sp<PipelineOutput> srcOutput = srcNode->getOutput(srcOutputId);
		if (srcOutput.isEmpty()) {
			FFL_LOG_WARNING("Pipeline::connect fail . not find node (%d)", srcNodeId);
			return FFL_INVALID_PARAMS;
		}

		PipelineNode* dstNode = getNodeManager()->findNode(dstNodeId);
		if (!dstNode) {
			FFL_LOG_WARNING("Pipeline::connect fail . not find dest node (%d)", dstNodeId);
			return FFL_INVALID_PARAMS;
		}

		sp<PipelineInput> dstInput = dstNode->getInput(dstInputId);
		if (dstInput.isEmpty()) {
			FFL_LOG_WARNING("Pipeline::connect fail . not find dest node (%d)", dstNodeId);
			return FFL_INVALID_PARAMS;
		}		

		srcOutput->connect(conn);
		dstInput->connect(conn);
		return FFL_OK;
	}

	//
	//  连接一个源节点
	//
	status_t Pipeline::connectSource(PipelineNodeId nodeId, PipelineInputId inputId,
		sp<PipelineSourceConnector> conn) {	    	
			if (conn.is_empty()) {
				return FFL_INVALID_PARAMS;
			}

			PipelineNode* dstNode = getNodeManager()->findNode(nodeId);
			if (!dstNode) {
				FFL_LOG_WARNING("Pipeline::connect fail . not find node (%d)", nodeId);
				return FFL_INVALID_PARAMS;
			}

			sp<PipelineInput> dstInput = dstNode->getInput(inputId);
			if (dstInput.isEmpty()) {
				FFL_LOG_WARNING("Pipeline::connect fail . not find dest node (%d)", inputId);
				return FFL_INVALID_PARAMS;
			}

			dstInput->connect(conn);
			return FFL_OK;
	}
	//
	//断开输出，输入的连接
	//
	status_t Pipeline::disconnect(PipelineNodeId nodeId, PipelineOutputId srcOutputId) {
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("destroyOutput fail . not find node (%d)", nodeId);
			return FFL_ERROR_FAIL;
		}

		sp<PipelineOutput> output = node->getOutput(srcOutputId);
		if (!output.isEmpty())
			output->disconnect();
		
		return FFL_OK;
	}
	//
	//断开nodeid的所有的输入接口
	//
	status_t Pipeline::disconnectInput(PipelineNodeId nodeId) {
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("disconnectInput fail . not find node (%d)", nodeId);
			return FFL_ERROR_FAIL;
		}

		Vector< sp<PipelineInput> > list;
		node->getStartedInputVector(list);

		uint32_t count = list.size();
		for (uint32_t i = 0; i < count; i++) {
			sp<PipelineInput> input = list[i];
			if (!input.isEmpty())
			   input->disconnect();
		}

		return FFL_OK;
	}
	//
	//断开nodeid的所有的输出接口
	//
	status_t Pipeline::disconnectOutput(PipelineNodeId nodeId) {
		PipelineNode* node = getNodeManager()->findNode(nodeId);
		if (!node) {
			FFL_LOG_WARNING("disconnectInput fail . not find node (%d)", nodeId);
			return FFL_ERROR_FAIL;
		}

		Vector< sp<PipelineOutput> > list;
		node->getOutputVector(list);
		uint32_t count = list.size();

		for (uint32_t i = 0; i < count; i++) {
			sp<PipelineOutput> output = list[i];
			if (!output.isEmpty())
			   output->disconnect();
		}

		return FFL_OK;
	}
	//
	//  是否工作中
	//
	bool Pipeline::isLooping()
	{
		return mFlags.hasFlags(LOOPING);
	}

	//
	//   设置获取track存储器，保证在启动前设置，运行中设置可能出现未知问题
	//
	void Pipeline::setTrackStorage(PipelineTrackStorage* storage) {
		mTrackStorage = storage;
	}
	PipelineTrackStorage* Pipeline::getTrackStorage() const{
		return mTrackStorage;
	}
}
