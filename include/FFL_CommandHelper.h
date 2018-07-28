/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_CommandHelper.h   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/21 
*  https://github.com/zhenfei2016/FFL-v2.git
*  命令行处理的帮助类
*
*   static void play(const char* args, void* userdata) {
*	}
*
*
*	static void stop(const char* args, void* userdata) {
*	}
*   static CmdOption  myCmdOption[] = {
*	   { "play",0,play,"play video " },
*	   { "stop",0,stop,"stop video " },
*	   { 0,0,0,0 }
*	};
*
*	int QuitFlag(void* userdata){
*	   return 0;
*	}
*
*	main(){
*	   FFL_inputLooper(myCmdOption,0,QuitFlag);
*	}
*
*
*/

#ifndef _FFL_COMMANDHELPER_H_
#define _FFL_COMMANDHELPER_H_


#include <FFL_Commandline.h>

#ifdef  __cplusplus
extern "C" {
#endif

	/*
	*  fnQuitFlag 返回非0则退出循环
	*/
	void FFL_inputLooper(CmdOption* opts, void* userdata,
		    int(*fnQuitFlag)(void* userdata) );

	/*
	*  打印帮助
	*/
	void FFL_cmdUsage(CmdOption* opts );

#ifdef  __cplusplus
}
#endif


#endif