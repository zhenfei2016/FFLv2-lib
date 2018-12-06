 /*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_Net.c
 *  Created by zhufeifei(34008081@qq.com) on 2017/8/12.
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *  网络socket公用函数
 *
 */

#include "internalSocket.h"
#include <net/base/FFL_Net.h>


static int gSocketInited=0;

void FFL_socketInit(){
	if (!gSocketInited) {
		SOCKET_SETUP();
		gSocketInited++;
	}
}
void FFL_socketUninit(){
	if (gSocketInited) {
		SOCKET_CLEANUP();
		gSocketInited=0;
	}
}
/*
*  创建socket
*/
NetFD FFL_socketCreate(int type) {
	NetFD fd = socket(AF_INET, type, 0);

	if (fd > 0) {
#ifdef ANDROID
		int flags = fcntl(fd, F_GETFD);
		flags |= FD_CLOEXEC;
		fcntl(fd, F_SETFD, flags);
#endif
	}
	return fd;
}
/*
*  关闭socket
*/
void FFL_socketClose(NetFD fd) {
	FFL_SOCKET_CLOSE(fd);
}

//static void fdCloseExec(NetFD fd)
//{
//#ifndef WIN32
//    int flags = fcntl(fd, F_GETFD);
//    flags |= FD_CLOEXEC;
//    fcntl(fd, F_SETFD, flags);
//#endif
//}


/*
*  accept一个客户端上来
*/
SOCKET_STATUS FFL_socketAccept(NetFD serverfd, NetFD* clientFd){
	int err = FFL_SOCKET_OK;
	int socketErr = 0;
	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);
	int sockfd = accept(serverfd, (struct sockaddr *) &addr, (socklen_t*)&addrlen);
	if (sockfd < 0) {
		socketErr = SOCKET_ERRNO();
		FFL_LOG_WARNING("FFL_socketAccept error=%d", socketErr);
		//是否超时超时
		if (socketErr == SOCKET_AGAIN ||
			socketErr == SOCKET_ETIME ||
			socketErr == SOCKET_ECONNRESET) {
			err = FFL_ERROR_SOCKET_TIMEOUT;
		}else {
			err = FFL_ERROR_SOCKET_FAILED;
		}
	}

	if (clientFd) {
		*clientFd = sockfd;
	}
	return err;
}

/*
* 读 ,
* readed : 获取读取了数据量
* 失败返回，FFL_ERROR_SOCKET_XX
* 成功返回  FFL_ERROR_SUCCESS
* */
SOCKET_STATUS FFL_socketRead(NetFD fd, void* buffer, size_t size,size_t* readed){
	int socketError=0;
	int nbRead = recv(fd, buffer, size, 0);
	if (nbRead > 0) {
		if (readed) {
			*readed = nbRead;
		}
		return  FFL_SOCKET_OK;
	}

	if (nbRead < 0  )	{
		socketError =SOCKET_ERRNO();	
		FFL_LOG_WARNING("FFL_socketRead error=%d", socketError);
		//读写超时
		if (socketError == SOCKET_AGAIN ||
			socketError == SOCKET_ETIME ||
			socketError == SOCKET_ECONNRESET) {
			return FFL_ERROR_SOCKET_TIMEOUT;
		} else {
			return FFL_ERROR_SOCKET_READ_EX + socketError;
		}
	}

	/* 
	 *  服务端关闭了 
	 */
	if (nbRead == 0) {
		FFL_LOG_WARNING("FFL_socketRead nb_read=0 error=%d",SOCKET_ECONNRESET);
		return FFL_ERROR_SOCKET_READ_EX + SOCKET_ECONNRESET;
	}

	return FFL_ERROR_SOCKET_READ;
}
SOCKET_STATUS FFL_socketReadFrom(NetFD fd, void* buffer, size_t size, size_t* readed, char* srcIp, uint16_t* srcPort) {
	int socketError = 0;
	int fromlen = sizeof(struct sockaddr_in);
	struct sockaddr_in srcAddr;
	FFL_zerop(&srcAddr);

	int nbRead = recvfrom(fd, buffer, size, 0, (struct sockaddr *)(&srcAddr), &fromlen);
	if (nbRead > 0) {
		if (readed) {
			*readed = nbRead;
		}

		if (srcIp) {
			const char* peerIp = inet_ntoa(srcAddr.sin_addr);
			int maxLen = FFL_MIN((strlen(peerIp)+1),16);
			memcpy(srcIp, peerIp, maxLen);
		}

		if (srcPort) {
			*srcPort = ntohs(srcAddr.sin_port);
		}
		return  FFL_SOCKET_OK;
	}

	if (nbRead < 0) {
		socketError = SOCKET_ERRNO();
		FFL_LOG_WARNING("FFL_socketRead error=%d", socketError);
		//读写超时
		if (socketError == SOCKET_AGAIN ||
			socketError == SOCKET_ETIME ||
			socketError == SOCKET_ECONNRESET) {
			return FFL_ERROR_SOCKET_TIMEOUT;
		}
		else {
			return FFL_ERROR_SOCKET_READ_EX + socketError;
		}
	}

	/*
	*  服务端关闭了
	*/
	if (nbRead == 0) {
		FFL_LOG_WARNING("FFL_socketRead nb_read=0 error=%d", SOCKET_ECONNRESET);
		return FFL_ERROR_SOCKET_READ_EX + SOCKET_ECONNRESET;
	}

	return FFL_ERROR_SOCKET_READ;
}
/*
 * 写 ,
 * writed : 写成功了多少数据
 * 失败返回，FFL_ERROR_SOCKET_XX
 * 成功返回  FFL_ERROR_SUCCESS
 * */
SOCKET_STATUS FFL_socketWrite(NetFD fd, void* buffer, size_t size,size_t* writed){
	int socketError=0;
	int nbWrite = send(fd, buffer, size, 0);
	if (nbWrite > 0) {
		if (writed)
			*writed = nbWrite;
		return  FFL_ERROR_SUCCESS;
	}		

	if (nbWrite < 0  )	{
		socketError=SOCKET_ERRNO();
		FFL_LOG_WARNING("FFL_socketWrite error=%d", socketError);
		/*
		 * 读写超时
	 	 */
		if (socketError == SOCKET_AGAIN ||
			socketError == SOCKET_ETIME ||
			socketError == SOCKET_ECONNRESET) {
			return FFL_ERROR_SOCKET_TIMEOUT;
		}
		else {
			return FFL_ERROR_SOCKET_WRITE_EX + socketError;
		}
	}

	return FFL_ERROR_SOCKET_WRITE;
}

SOCKET_STATUS FFL_socketWriteTo(NetFD fd, void* buffer, size_t size, size_t* writed, const char* destIp, uint16_t destPort) {
	int socketError = 0;
	struct sockaddr_in destAddr;

	if (destIp == NULL || strlen(destIp) > 32) { 
		return FFL_FAILED;
	}

	destAddr.sin_family = AF_INET;      
	destAddr.sin_port = htons(destPort);	
	destAddr.sin_addr.s_addr = inet_addr(destIp);

	int nbWrite = sendto(fd, buffer, size, 0,(struct sockaddr*)&destAddr,sizeof(struct sockaddr_in));	
	if (nbWrite > 0) {
		if (writed)
			*writed = nbWrite;
		return  FFL_ERROR_SUCCESS;
	}	

	if (nbWrite < 0) {
		socketError = SOCKET_ERRNO();
		FFL_LOG_WARNING("FFL_socketWrite error=%d", socketError);
		/*
		* 读写超时
		*/
		if (socketError == SOCKET_AGAIN ||
			socketError == SOCKET_ETIME ||
			socketError == SOCKET_ECONNRESET) {
			return FFL_ERROR_SOCKET_TIMEOUT;
		}
		else {
			return FFL_ERROR_SOCKET_WRITE_EX + socketError;
		}
	}

	return FFL_ERROR_SOCKET_WRITE;
}
/*
*  设置接收超时值
*/
SOCKET_STATUS FFL_socketSetRecvTimeout(NetFD fd, int64_t ms)
{
	int32_t sec = 0;
	int32_t usec = 0;
	struct timeval tv = { (int32_t)(ms / 1000) , (int32_t)((ms % 1000)*1000) };
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,(const char*)( &tv), sizeof(tv)) == -1)
	{
		return FFL_ERROR_SOCKET_FAILED;
	}
	return FFL_SOCKET_OK;
}

/*
*  设置发送超时值
*/
SOCKET_STATUS FFL_socketSetSendTimeout(NetFD fd, int64_t ms){
	int32_t sec = 0;
	int32_t usec = 0;
	struct timeval tv = { (int32_t)(ms / 1000) , (int32_t)((ms % 1000)*1000) };
	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)(&tv), sizeof(tv)) == -1)
	{
		return FFL_ERROR_SOCKET_FAILED;
	}
	return FFL_SOCKET_OK;
}
/*
*  设置发送不延迟发送
*/
SOCKET_STATUS FFL_socketSetNodelay(NetFD fd, int yes)
{
#if defined(WIN32)
	int flag = (yes != 0) ? 1 : 0;
	int nSendBuf = 0;
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
#else
	int enable = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
#endif
	return FFL_SOCKET_OK;
}
/*
* 分解host和port
* www.123.com:4000
* host=www.123.com
* port=4000
*/
SOCKET_STATUS FFL_socketPaserHost(const char* hostport, char* host, int32_t* port){
	const char* src = hostport;
	while (*src) {
		if (*src == ':') {
			*port = atoi(src + 1);			
			break;
		}
		*host++ = *src++;
	}
	*host = 0;
	return FFL_OK;
}

/*
*  获取本地的地址，返回获取的数量
*  hostlist : 如果多个地址则使用;分割开
*/
int32_t FFL_socketLocalAddr(char* hostlist, int32_t bufSize) {
	char hostname[128];
	struct hostent *hent;
	struct in_addr* addr = 0;

	gethostname(hostname, sizeof(hostname));	
	hent = gethostbyname(hostname);
	if (0 == hent){
	    return 0;
	}	
	
	addr = ((struct in_addr*)hent->h_addr);
	strncpy(hostlist, inet_ntoa(*addr), bufSize - 1);
	return 1;
}



int32_t FFL_socketSelect(const NetFD *fdList, int8_t *flagList, size_t fdNum, int64_t timeoutUs) {
	struct timeval tv;
	NetFD maxfd = 64;
	fd_set fdset;
	size_t i = 0;
	int status = 0;
	int socketError = 0;

	if (timeoutUs > 0) {
		tv.tv_sec = (long)(timeoutUs / (1000 * 1000));
		tv.tv_usec = (long)(timeoutUs % (1000 * 1000));
	}
	else {
		tv.tv_sec = -1;
		tv.tv_usec = -1;
	}


#if WIN32
	if (fdNum > 64) {
		return FFL_ERROR_SOCKET_SELECT;
	}
#else
	maxfd = 0;
	for (i = 0; i < fdNum; i++) {
		if (fdList[i] > maxfd) {
			maxfd=fdList[i];
        }
    }
	maxfd+=1;
#endif
	
	FD_ZERO(&fdset);
	for ( i = 0; i < fdNum; i++) {
		FD_SET(fdList[i], &fdset);
		flagList[i] = 0;
	}

	status = select(maxfd, &fdset, 0, 0, (timeoutUs == 0 ? NULL : (&tv)));
	if (status < 0) {
#if WIN32
		return FFL_ERROR_SOCKET_SELECT;
#else
		socketError = SOCKET_ERRNO();
		if (socketError == EINTR) {
			//
			//  当做超时处理，可能其他信号触发了这
			//
			return 0;
		}
#endif
	}

	if (status > 0) {
		for (size_t i = 0; i < fdNum; i++) {
			if (FD_ISSET(fdList[i], &fdset))
				flagList[i] = 1;
		}
	}
	return status;
}


