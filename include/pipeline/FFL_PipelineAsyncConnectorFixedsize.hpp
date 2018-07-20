/*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineAsyncConnectorFixedsize.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2017/12/16
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *
 *  node异步连接器,进行消息处理中消息的缓存异步处理
 *  具有固定大小的connecter ，待处理的消息不会大于这个大小
 */

#ifndef _FFL_PIPELINE_ASYNCCONNECTOR_FIXSIZE_HPP_
#define _FFL_PIPELINE_ASYNCCONNECTOR_FIXSIZE_HPP_

#include <pipeline/FFL_PipelineAsyncConnector.hpp>
#include <utils/FFL_Flags.hpp>
#include <list>
#include <vector>

namespace FFL
{
	//
	//异步连接器   
    //
	class PipelineAsyncConnectorFixSize : public PipelineAsyncConnector {
	public:
		//
		//capacity: 这个的最大大小
		//looper : 这个异步连接器在哪一个looper中跑
		//		
		PipelineAsyncConnectorFixSize(uint32_t capacity);
		PipelineAsyncConnectorFixSize(uint32_t capacity, sp<PipelineLooper> looper);
		virtual  ~PipelineAsyncConnectorFixSize();

		//
		//  请求shutdown,等待shutdown结束
		//
		status_t requestShutdown();
		status_t waitShutdown();

		//
		// 获取，设置容量 
		//
		void setCapacity(uint32_t capacity);
		uint32_t getCapacity() const;
		//
		// 连接器转发消息
		//
		status_t tranport(const sp<PipelineMessage> &msg, int64_t delayUs);
		//
		//  清空转发的消息
		//
		void clearMessage();
		//
		//  消耗了这条消息
		//
		void consumer(const sp<PipelineMessage> &msg);		
		//
		// 获取待处理的消息的大小
		//
		int64_t getPendingMessageSize();
	private:
		//
		//  重新计算一下现在消息的大小
		//
		void reCalcSizeLock();
	protected:
		CCondition mCond;
		mutable CMutex mMutex;
		volatile bool mHasShutdownRequest;
		//
		//  当前缓存的消息列表大小， 容量
		//
		uint32_t mSize;
		uint32_t mCapacity;
		bool mIsFull;
		//
		//  缓存的待处理消息列表
		//
		typedef std::list < sp<PipelineMessage> > MessageList;
		MessageList mMessageListCache;
	public:
		enum FullFlag{
			//
			//  如果满了情况下，tranport需要等待
			//
			IF_FULL_WAIT = 0,
			//
			//  如果满了情况下，tranport不等待，移除第一条再插入到最后
			//  第一条指的是最先处理的那一条
			//
			IF_FULL_REMOVE_FIRST=1,
			//
			//  如果满了情况下，tranport不等待，移除最后一条再插入到最后
			//
			IF_FULL_REMOVE_LAST =2,
		};
		//
		// 设置满了情况的处理标志
		//
		void setFullFlag(PipelineAsyncConnectorFixSize::FullFlag flag);

		//
		//  消息列表的总大小计算器
		//  消息列表是否满了，是根据这个类进行计算的
		//
		class MessageSizeCalculator {
		public:
            virtual ~MessageSizeCalculator(){}
			//
			//  这个仅仅计算消息列表的大小，不要加一些复杂的过程操作
			//
			virtual  uint32_t calcSize(std::list< sp<PipelineMessage> >& msgList)=0;			
		};
		//		
		//  设置消息列表的总大小计算器
		//  这个函数需要在connect启动前进行设置,启动后设置可能会出现未知异常
		//  calc: 类释放是，会进行delete calc
		//
		void setMessageSizeCalculator(MessageSizeCalculator* calc);
	private:
        //
		//  当满的时候，怎么处理的标志
		//
		Flags32b mFullFlags;
		//
		//  设置消息列表的总大小计算器
		//
		MessageSizeCalculator* mMessageSizeCalculator;
	};

	
}


#endif
