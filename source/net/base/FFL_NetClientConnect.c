 /*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_NetClient.c
 *  Created by zhufeifei(34008081@qq.com) on 2017/8/12.
 *  https://github.com/zhenfei2016/FFL-v2.git
  *  连接指定服务器，参考android adb代码
 *
 */
#include "internalSocket.h"
#include <net/base/FFL_Net.h>

SOCKET_STATUS FFL_socketNetworkClient(const char *host, int port, int type, NetFD*fd )
{
    return FFL_socketNetworkClientTimeout(host,port,type,fd,0);
}


SOCKET_STATUS FFL_socketNetworkClientTimeout(const char *host, int port, int type, NetFD* fd, int timeout_tm)
{
    struct hostent *hp;
    struct sockaddr_in addr;
    int s;

    hp = gethostbyname(host);
	if (hp == 0) {
		return FFL_ERROR_SOCKET_GET_PEER_IP;
	}

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = hp->h_addrtype;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

    s = socket(hp->h_addrtype, type, 0);
	if (s < 0) {
		return FFL_ERROR_SOCKET_CREATE;
	}

    int error=connect(s, (struct sockaddr *) &addr, sizeof(addr));
    if( error< 0){
        FFL_socketClose(s);
        return FFL_ERROR_SOCKET_CONNECT;
    }

	*fd = s;
    return FFL_SOCKET_OK;
}


