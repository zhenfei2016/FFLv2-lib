/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_CommandHelper.c
*  Created by zhufeifei(34008081@qq.com) on 2018/07/21
*  https://github.com/zhenfei2016/FFL-v2.git
*  �����д���İ�����
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
*/
#include <FFL_CommandHelper.h>

static void help(const char* args, void* userdata);
static CmdOption  gCmdOption[] = {
	{ "help",0,help,"printf help" },
	{ 0,0,0,0 }
};

static void help(const char* args, void* userdata) {
	int32_t i = 0;
	CmdOption* opts =(CmdOption*) userdata;
	printf("--------------------user usage-----------------------------------------\n");
    while(opts->mName){
		printf("%s  :%s \n", opts->mName, opts->nHelp? opts->nHelp:"");
		opts++;
	}

	printf("--------------------sys usage-----------------------------------------\n");
	i = 0;
	opts = gCmdOption;
	while (opts->mName) {
		printf("%s  :%s \n", opts->mName, opts->nHelp ? opts->nHelp : "");
		opts++;
	}
	printf("----------------------------------------------------------------------\n");
}

/*
*  fnQuitFlag ���ط�0���˳�ѭ��
*/
void FFL_inputLooper(CmdOption* opts,void* userdata,
	int(*fnQuitFlag)(void* userdata)){
	char cmd[256] = {0};
	cmd[0] = '-';
	cmd[1] = '-';
	const char *argv[] = {
		"",
		cmd,
	};
	int argc = 2;

	int optCount = 0; 
	{
		CmdOption* opt = opts;		
		while (opt->mName) {
			optCount++;
			opt++;
		}
	}

	while (fgets(cmd + 2, 256 - 3, stdin)) {
		//
		//  �����������ʽ��Ϊ  cmd=xxx  ,���ǰ�����ת���ɵ�һ������
		//		
		char* pCmdLine = cmd + 2;
		while (*pCmdLine++) {
			if (*pCmdLine == ' ' || *pCmdLine == '\n' || *pCmdLine == '\r') {
				*pCmdLine = '=';
				break;
			}
		}

		if (FFL_parseCommnadline(argc, argv,
			opts,
			optCount,
			userdata) < 0) {
			/*
			*  ��һ�±�ϵͳ�Ƿ�֧���������
			*/
			if(FFL_parseCommnadline(argc, argv,
				gCmdOption,
				FFL_ARRAY_ELEMS(gCmdOption),
				opts) < 0){
				printf("unknown command: %s \n", cmd + 2);
			}		
		}

		if (fnQuitFlag && fnQuitFlag(userdata)!=0) {
			break;
		}
	}
}


/*
*  ��ӡ����
*/
void FFL_cmdUsage(CmdOption* opts) {
	help(0,opts);
}