 /*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_socketLoopbackClient.c
 *  Created by zhufeifei(34008081@qq.com) on 2017/8/12.
 *  https://github.com/zhenfei2016/FFL-v2.git
 *  连接当前机子的环回地址(127.0.0.1)，参考android adb代码
 *
 */
#include "internalSocket.h"
#include <net/base/FFL_Net.h>

SOCKET_STATUS FFL_socketLoopbackClient(int port, int type, NetFD*fd)
{
    return FFL_socketLoopbackClientTimeout(port,type,fd,0);
}

SOCKET_STATUS FFL_socketLoopbackClientTimeout(int port, int type, NetFD*fd,int timeout_tm){
    struct sockaddr_in addr;
    int s;
    FFL_zerop(&addr);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    s = socket(AF_INET, type, 0);
	if (s < 0) {
		return FFL_ERROR_SOCKET_CREATE;
	}

    if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        FFL_socketClose(s);
        return FFL_ERROR_SOCKET_CONNECT;
    }

	*fd = s;
    return FFL_SOCKET_OK;
}
