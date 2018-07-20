/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_Singleton.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/06/22 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *  
 *  单例模板
 *  example:
 *  .hpp
 *  class A : public Singleton<A>{
 *    public:
 *       void test();
 *  }
 *  
 *  .cpp
 *  FFL_IMPLEMENT_SINGLETON(A);
 *  void A::test(){}
 *
 *  usage:
 *  A::getInstance().test();
 *  
*/
#ifndef _FFL_SINGLE_INSTANCE_HPP_
#define _FFL_SINGLE_INSTANCE_HPP_

#include <FFL.h>
#include <utils/FFL_Utils.hpp>
#include <thread/FFL_Mutex.hpp>

namespace FFL{	
	//
	//  所有单例类公用的一个锁
	//
	CMutex& getSingletonLock();

	template < typename T >
	class Singleton {
		protected:
			Singleton() {			
			}
			~Singleton() {}
		public:
			//
			//  获取单例
			//
			static T &getInstance(){
				if (sInstance != NULL) {
					return *sInstance;
				}				

				CMutex::Autolock l(getSingletonLock());
				if (sInstance) {
					return *sInstance;
				}
				sInstance = new T();				
				return *sInstance;
			}
			//
			//  释放单例对象，不建议调用这个，
			//  如果一定要调用他，你需要确定 getInstance()获得的单例对象没有在使用中，getInstance()没有正在调用中
			//  
			//
			static void deleteInstance() {				
				CMutex::Autolock l(getSingletonLock());
				if (sInstance) {
					delete sInstance;
					sInstance = NULL;
				}
			}
		protected:
			static T* sInstance;				
			DISABLE_COPY_CONSTRUCTORS(Singleton);
	};
#ifdef WIN32
   #define FFL_IMPLEMENT_SINGLETON(ClassName) ClassName* FFL::Singleton< ClassName >::sInstance=0
#else
   #define FFL_IMPLEMENT_SINGLETON(ClassName) template<> ClassName* FFL::Singleton< ClassName >::sInstance=0
#endif
  
	
}
#endif

