/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Net.h
*  Created by zhufeifei(34008081@qq.com) on 2017/8/12.
*  https://github.com/zhenfei2016/FFL-v2.git
*  网络socket公用函数
*
*/

#ifndef _FFL_SOCKET_H_
#define _FFL_SOCKET_H_

#include <FFL.h>
#include <net/base/FFL_NetConst.h>

#ifdef WIN32
#include <winsock2.h>
typedef int  socklen_t;
#else
#include <sys/socket.h>
#endif

#ifdef __cplusplus 
extern "C" {
#endif
	/* socket库初始化 */
	void FFL_socketInit();
	void FFL_socketUninit();
    /*
	 *连接到loopback的服务器，返回句柄 
	 */
	SOCKET_STATUS FFL_socketLoopbackClient(int port, int type, NetFD*fd);
	SOCKET_STATUS FFL_socketLoopbackClientTimeout(int port, int type, NetFD*fd,int timeout_tm);
    #define FFL_socketLoopbackTcpClient(port,fd) FFL_socket_loopback_client(port,SOCK_STREAM,fd)
    #define FFL_socketLoopbackUdpClient(port,fd) FFL_socket_loopback_client(port,SOCK_DGRAM,fd)
    /*  
	 * 本地loopback的服务器，监听连接
	 */
	SOCKET_STATUS FFL_socketLoopbackServer(int port, int type,NetFD* fd);
    #define FFL_socketLoopbackTcpServer(port,fd) FFL_socket_loopback_server(port,SOCK_STREAM,fd);  
    #define FFL_socketLoopbackUdpServer(port,fd) FFL_socket_loopback_server(port,SOCK_DGRAM,fd);·
	/*
	 *连接到服务器，返回句柄fd
	 */
	SOCKET_STATUS FFL_socketNetworkClient(const char *host, int port, int type, NetFD* fd);
	SOCKET_STATUS FFL_socketNetworkClientTimeout(const char *host, int port, int type, NetFD* fd,int timeout_tm);
	#define FFL_socketNetworkTcpClient(host,port,fd) FFL_socket_network_client(host,port,SOCK_STREAM,fd)
	#define FFL_socketNetworkUdpClient(host,port,fd) FFL_socket_network_client(host,port,SOCK_DGRAM,fd)

	/* 
	 *本地的服务器，监听连接 
	 */
	SOCKET_STATUS FFL_socketAnyAddrServer(int port, int type,int* fd);
	#define FFL_socketAnyAddrTcpServer(port,fd) FFL_socketAnyAddrServer(port,SOCK_STREAM,fd)
	#define FFL_socketAnyAddrUdpServer(port,fd) FFL_socketAnyAddrServer(port,SOCK_DGRAM,fd)

	/* 
	 *关闭句柄 
	 */
	void FFL_socketClose(NetFD fd);

    /*  
	 * 服务端accept一个客户端，当一个客户端连接上来的时候，这个会返回的  clientfd
	 */
	SOCKET_STATUS FFL_socketAccept(NetFD serverfd, NetFD* clientfd);
	    
    /*
     * 网络读 
     * readed : 获取读取了数据量
     * 失败返回，FFL_ERROR_SOCKET_XX
     * 成功返回  FFL_ERROR_SUCCESS
     * */
	SOCKET_STATUS FFL_socketRead(NetFD fd, void* buffer, size_t size,size_t* readed);

	/*
	 * 网络写 
	 * writed : 写成功了多少数据
	 * 失败返回，FFL_ERROR_SOCKET_XX
	 * 成功返回  FFL_ERROR_SUCCESS
	 * */
	SOCKET_STATUS FFL_socketWrite(NetFD fd, void* buffer, size_t size,size_t* writed);

    /*
	 * 设置发送，接收超时时间
	 */
	SOCKET_STATUS FFL_socketSetRecvTimeout(NetFD fd, int64_t ms);
	SOCKET_STATUS FFL_socketSetSendTimeout(NetFD fd, int64_t ms);
	/* 
	 *  设置不延迟发送 (禁用粘包)
	 */
	SOCKET_STATUS FFL_socketSetNodelay(NetFD fd, int yes);
#ifdef __cplusplus 
}
#endif


#endif
