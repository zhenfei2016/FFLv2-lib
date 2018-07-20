/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Message.hpp
*  Created by zhufeifei(34008081@qq.com) on 2017/11/25
*  参考 Android Open Source Project 进行修改的
*  Looper，Handler处理的Message
*
*/
#ifndef _FFL_MESSAGE_HPP_
#define _FFL_MESSAGE_HPP_

#include <utils/FFL_Serializable.hpp>
#include <utils/FFL_Looper.hpp>
#include <thread/FFL_SysMessage.hpp>
#include <utils/FFL_String.hpp>


namespace FFL {
	class PipelineMessage;

	class Serializable;
    class Message : public Serializable
    {
    public:
        Message(uint32_t what = 0, Looper::handler_id target = 0);
		//
		//  唯一id
		//
		uint32_t uniqueId() const;
		//
		//  更新一下唯一id
		//
		void updateUniqueId();

        //
        //标识什么消息
		//
        void setWhat(uint32_t what);
        uint32_t what() const;

        //
        //设置消息的处理handler的句柄
        //
        void setTarget(Looper::handler_id target);
        Looper::handler_id target() const;

		//
        //发送这个消息
		//
       // void post(int64_t delayUs = 0);

        // Posts the message to its target and waits for a response (or error)
        // before returning.
        //发送这个消息并且等待应答
        //
        //status_t postAndAwaitResponse(sp<Message> *response);

        // If this returns true, the sender of this message is synchronously
        // awaiting a response, the "replyID" can be used to send the response
        // via "postReply" below.
        //获取这个消息是否发送端还在等待中
        //如果返回true.则replyID为回复id,
        // 处理完成以后需要postReply
        //
        //bool senderAwaitsResponse(uint32_t *replyID) const;

		//
        //表示这个消息为应答消息
        //发送应答
		//
        //void postReply(uint32_t replyID);

    public:
		//
        //清空消息的内容
		//
        void clear();

		//
        //copy这个消息
		//
        sp<Message> dup() const;

        std::string debugString(){ return "";}
    protected:
        virtual ~Message();
    private:
		//
		//  消息的唯一id,每次创建的消息的id都是不同的
		//
		uint32_t mUniqueId;
		//
		//  这是什么消息
		//
        uint32_t mWhat;
        Looper::handler_id mTarget;

        friend class LooperRoster;
		//
        //发送者发送的消息，
        //如果在等待应答，则会设置这个字段
        //0:表示无效的应答id
		//
        uint32_t mSenderAwaitsReplyID;

        DISABLE_COPY_CONSTRUCTORS(Message);
	public:		
		sp<Serializable> mObj;
    };
    
   
}
#endif
