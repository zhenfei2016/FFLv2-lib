/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PipelineMessageCache.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  PipelineMessageCache用于缓存特定类型的消息
*
*/
#ifndef __FFL_PIPELINE_MESSAGE_CACHE_HPP_
#define __FFL_PIPELINE_MESSAGE_CACHE_HPP_


#include <pipeline/FFL_PipelineMessage.hpp>

namespace FFL{

	//
	//   缓存一些消息，必输是同一类型的
	//
	class PipelineMessageCache : public RefBase {
	public:
		//
		//  msgType:缓存的消息类型
		//
		PipelineMessageCache(uint32_t msgType);
		~PipelineMessageCache();

		//
		//  缓冲这条PipelineMessage
		//
		void cache(const sp<PipelineMessage>& msg);

		//
		//  dump出来一条PipelineMessage，如果缓存中不存在则返回NUll
		//
		sp<PipelineMessage> dump();
		//
		//  dump出来一条PipelineMessage，如果缓存中不存在则new出来
		//
		sp<PipelineMessage> dumpIfNullAlloc();

		//
		//  清空缓存
		//
		void clear();
	public:
		//
		//  cache中创建消息
		//
		template<class PLAYLOAD>
		FFL::sp<FFL::PipelineMessage> createMessage(PLAYLOAD** payload,int32_t playloadType){
			FFL::sp<FFL::PipelineMessage> msg = dumpIfNullAlloc();
			PLAYLOAD* pl = NULL;
			//
			//  获取缓存
			//
			do {
				FFL::PipelineMessagePayload* payload = msg->getPayload();
				if (payload != NULL && msg->getType() == playloadType) {
					pl = (PLAYLOAD*)payload;
				}

				if (pl) break;

				//
				//  如果不存在则创建他
				//
				pl = new PLAYLOAD();
				msg->setPayload(pl);
			} while (0);

			*payload = pl;
			return msg;
		}
	private:
		class CacheListener;
		friend class CacheListener;
		CacheListener* mListener;
		
	protected:
		CMutex mListMutex;
		std::list< sp<PipelineMessage> > mList;
        
        std::list< sp<PipelineMessage> > mDumpdList;

		uint32_t mMsgType;
	};

	
}


#endif
