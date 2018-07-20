/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Message
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  https://github.com/zhenfei2016/FFLv2-lib.git
*  参考 Android Open Source Project 进行修改的
*  Looper，Handler处理的Message
*
*/
#include <utils/FFL_Message.hpp>
#include <pipeline/FFL_PipelineMessage.hpp>

namespace FFL {   

	static uint32_t gUniqueId=1;
	static void generateUniqueid(uint32_t& id) {
		id= gUniqueId++;
	}

    Message::Message(uint32_t what, Looper::handler_id target)
        : mWhat(what),
          mTarget(target),
          mSenderAwaitsReplyID(0)		  
    {
		generateUniqueid(mUniqueId);
	}

    Message::~Message() {
        clear();
    }

	uint32_t Message::uniqueId() const
	{
		return mUniqueId ;
	}
	//
	//  更新一下唯一id
	//
	void Message::updateUniqueId()
	{
		generateUniqueid(mUniqueId);		 
	}
    void Message::setWhat(uint32_t what) {
        mWhat = what;
    }

    uint32_t Message::what() const {
        return mWhat;
    }

    void Message::setTarget(Looper::handler_id handlerID) {
        mTarget = handlerID;
    }

    Looper::handler_id Message::target() const {
        return mTarget;
    }

    void Message::clear(){
    }

  //  void Message::post(int64_t delayUs){
  //     // gLooperRoster.postMessage(this, delayUs);
  //  }

  //  status_t Message::postAndAwaitResponse(sp<Message> *response) {
  //      //return gLooperRoster.postAndAwaitResponse(this, response);
		//return 0;
  //  }

  //  void Message::postReply(uint32_t replyID) {
  //      //gLooperRoster.postReply(replyID, this);
  //  }

    //bool Message::senderAwaitsResponse(uint32_t *replyID) const {
    //    if(mSenderAwaitsReplyID!=0){
    //        *replyID =mSenderAwaitsReplyID;
    //        return true;
    //    }
    //    return false;
    //}

    sp<Message> Message::dup() const {
        sp<Message> msg = new Message(mWhat, mTarget);
        return msg;
    }	
}
