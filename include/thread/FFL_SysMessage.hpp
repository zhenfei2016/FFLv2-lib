

#ifndef _FFL_SYS_MESSAGE_HPP_
#define _FFL_SYS_MESSAGE_HPP_

//
//  系统消息
//
namespace FFL {	

	enum 
	{
		MSGT_PIPELINE=7000,
		//
		//  系统消息开始
		//
		MSGT_SYS_BEGIN=8000,
        //
        //开始looper的消息循环
        //
        MSGT_SYS_START_LOOPER=8887,
		//
		//  系统消息类型， 退出Looper
		//
		MSGT_SYS_QUIT_LOOPER=8888,
		//
		//  系统消息结束
		//
		MSGT_SYS_END= 9000,
	};
}
#endif
