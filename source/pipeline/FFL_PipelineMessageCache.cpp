/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_PipelineMessageCache.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/10 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  PipelineMessageCache用于缓存特定类型的消息
 *
*/
#include <pipeline/FFL_PipelineMessageCache.hpp>

namespace FFL {
	class PipelineMessageCache::CacheListener: public MessageConsumeListener {
	public:
        virtual ~CacheListener(){
        }
        
		virtual void onConsume(FFL::PipelineMessage* msg) {
			sp<PipelineMessageCache> cache=mCache.promote();
			if(!cache.isEmpty()){
				cache->cache(msg);
			}else {
				FFL_LOG_WARNING("PipelineMessageCache has freed");
			}
		}
		wp<PipelineMessageCache> mCache;
	};

	PipelineMessageCache::PipelineMessageCache(uint32_t msgType):
		mMsgType(msgType){
		mListener=new CacheListener();
		mListener->mCache = this;
	}
	PipelineMessageCache::~PipelineMessageCache(){
		{
			CMutex::Autolock l(mListMutex);
			if (mDumpdList.size() > 0) {
				FFL_LOG_WARNING("PipelineMessageCache not empty!");
			}
		}
		delete mListener;
		mListener = NULL;
	}

	//
	//  缓冲这条PipelineMessage
	//
	void PipelineMessageCache::cache(const sp<PipelineMessage>& msg)
	{
		if (msg->getType() != mMsgType)
		{
			FFL_LOG_WARNING(" different message type (%d %d)",msg->getType(),mMsgType);
			return;
		}
		CMutex::Autolock l(mListMutex);
		mList.push_back(msg);        
        for(std::list< sp<PipelineMessage> >::iterator it=mDumpdList.begin();
            it!=mDumpdList.end();it++){
            if((*it).get()==msg.get()){
                mDumpdList.erase(it);
				break;
            }
        }
	}

	//
	//  dump出来一条PipelineMessage，如果缓存中不存在则返回NUll
	//
	sp<PipelineMessage> PipelineMessageCache::dump() {
		CMutex::Autolock l(mListMutex);
		if (mList.size() > 0) {
			sp<PipelineMessage>  msg=mList.front();
			mList.pop_front();
            mDumpdList.push_back(msg);
			return msg;
		}
		return NULL;
	}
	//
	//  dump出来一条PipelineMessage，如果缓存中不存在则new出来
	//
	sp<PipelineMessage> PipelineMessageCache::dumpIfNullAlloc(){
        CMutex::Autolock l(mListMutex);
        sp<PipelineMessage>  msg=0;
        if (mList.size() > 0) {
            msg=mList.front();
            mList.pop_front();
        }
        
		if (msg.isEmpty()) {
			msg = new PipelineMessage(mMsgType);
			msg->setConsumeListener(mListener);
            FFL_LOG_DEBUG("PipelineMessageCache new PipelineMessage(%d) cache:%d dumped:%d",mMsgType,mList.size(),mDumpdList.size()+1);
		}
        mDumpdList.push_back(msg);
		return msg;
	}

	//
	//  清空缓存
	//
	void PipelineMessageCache::clear()
	{
		CMutex::Autolock l(mListMutex);
		mList.clear();
        mDumpdList.clear();
	}
}
