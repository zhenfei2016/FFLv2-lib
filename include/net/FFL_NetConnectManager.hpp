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
#include <thread/FFL_Mutex.hpp>

namespace FFL {
	class NetConnect;
	class NetServer;

	class NetConnectManager {
	public:
		NetConnectManager();
		virtual ~NetConnectManager();	

		virtual NetConnect* onCreateConnect(NetFD fd, NetServer* srv) = 0;
		virtual void onDestroyConnect(NetConnect* conn) = 0;

	protected:
		friend class NetServer;
		friend class NetConnect;
		//
		//  创建连接，删除连接
		//
		NetConnect* createConnect(NetFD fd, NetServer* srv);
		void destroyConnect(NetFD fd);	
	private:
		//
		//  list中添加删除指定的conn
		//
		void addConnect(NetConnect* conn);
		NetConnect* removeConnect(NetFD fd);
	protected:
		CMutex mConnLock;
		List<NetConnect*> mConnects;
	};


	template<typename CONN>
	class SimpleConnectManager : public NetConnectManager {
	public:
		SimpleConnectManager() {}
		virtual ~SimpleConnectManager() {}
	protected:
		virtual NetConnect* onCreateConnect(NetFD fd, NetServer* srv) {
			return new CONN(fd, srv);
		}
		virtual void onDestroyConnect(NetConnect* conn) {
			if (conn) {
				delete conn;
			}
		}
	};
}

#endif