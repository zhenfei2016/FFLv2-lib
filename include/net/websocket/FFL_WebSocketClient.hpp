/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_WebSocketClient.hpp
*  Created by zhufeifei(34008081@qq.com) on 2019/02/24
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  websocket client
*/

#ifndef _FFL_WEBSOCKET_CLIENT_HPP_
#define _FFL_WEBSOCKET_CLIENT_HPP_
#include <net/FFL_TcpClient.hpp>

namespace FFL {		
	class FFLIB_API_IMPORT_EXPORT WebSocketClient : public TcpClient {
	public:
		WebSocketClient(const char* ip, int32_t port);
		virtual ~WebSocketClient();
	protected:	
		//
		//  握手
		//
		bool handshark();
	};
}
#endif
