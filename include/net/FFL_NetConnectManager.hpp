/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_NetConnectManager.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  连接管理器，创建保存连接
*/
#ifndef _FFL_NETCONNECT_MANAGER_HPP_
#define _FFL_NETCONNECT_MANAGER_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>
#include <thread/FFL_Mutex.hpp>

namespace FFL {
	class NetConnect;
	class NetServer;

	class NetConnectManager {
	public:
		NetConnectManager();
		virtual ~NetConnectManager();	
	public:
		virtual NetConnect* createConnect(NetFD fd, NetServer* srv)=0;
		virtual void destroyConnect(NetFD fd)=0;
	protected:
		//
		//  list中添加删除指定的conn
		//
		void addConnect(NetConnect* conn);
		NetConnect* removeConnect(NetFD fd);
	protected:
		CMutex mConnLock;
		List<NetConnect*> mConnects;
	};
}

#endif