/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Commandline.c
*  Created by zhufeifei(34008081@qq.com) on 2018/06/19
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  分析命令行帮助函数
*
*/
#include <FFL_Commandline.h>
#include "getopt/getopt.h"

static struct option* alloc_getopt_Option(CmdOption* opts, int size) {	
	struct option* optionList = FFL_mallocz((size +1)* sizeof(struct option) );
	for (int i = 0; i < size; i++) {
		if (!opts[i].mName) {
			opts[i].mName = "__dummy__";
			continue;
		};

		optionList[i].name = FFL_strdup(opts[i].mName);
		optionList[i].has_arg = optional_argument;//(opts[i].mHaveAargument !=0)? optional_argument : no_argument;
		optionList[i].flag = NULL;
		optionList[i].val = 0;		
	}
	return optionList;
}

static void free_getopt_Option(struct option* opts) {
	struct option* opt = opts;
	while (opt->name)
	{
		FFL_free((void *)opt->name);
		opt++;
	}

	FFL_free(opts);
}

/*
*   分析命令行
*   argc：参数个数
*   argv：参数数组
*   opts：支持的命令数组，以null结尾
*   size：opts数组的大少
*   命令行格式  --cmd=12344
*   返回命令在opts中的位置，没找到返回-1
*/
int FFL_parseCommnadline(int argc,const char** argv,CmdOption* opts, int size, void* userdata) {
	struct option* longOpt=alloc_getopt_Option(opts, size);
	
	int cmdIndex = -1;
	int optionIndex=-1;
	int opt = -1;
	optind = 1;	
	while ((opt = getopt_long_only(argc, 
		         ( char *const *)argv,
		         "",
		         longOpt,
		         &optionIndex)) != -1)
	{
		if (optionIndex != -1) {
			cmdIndex = optionIndex;
			if (opts[cmdIndex].fun) {
				opts[cmdIndex].fun(optarg,userdata);
			}
		}
		optionIndex = -1;
	}
	free_getopt_Option(longOpt);

	return cmdIndex;
}
