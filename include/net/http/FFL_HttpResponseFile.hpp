/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponseFile.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  http应答文件请求的
*/
#ifndef _FFL_HTTP_RESPONSE_FILE_HPP_
#define _FFL_HTTP_RESPONSE_FILE_HPP_

#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpStreamCallback.hpp>
#include <FFL_Io.hpp>

namespace FFL {	

	class HttpClient;
	class FFLIB_API_IMPORT_EXPORT HttpResponseFile : public HttpResponse,
	   public HttpStreamCallback  {
	protected:
		friend class HttpResponseBuilder;
		HttpResponseFile(FFL::sp<HttpClient> client);		
	public:
		virtual ~HttpResponseFile();		

		bool setReader(FFL::IOReader* reader);
		bool guessFileType(const char* fileName);

		bool setWriter(FFL::IOWriter* writer,uint32_t size);
	protected:
		virtual bool writeContent();
	protected:
		/*   HttpStreamCallback  */
		//
		//  拉取数据，读操作
		//  data:外部提供的缓冲区
		//  bufLen:外部提供的缓冲区大小
		//  size:实质上填充了多少
		//  返回false表示失败
		//
		virtual bool onReadData(char* data, size_t bufLen, size_t* size);
		//
		//  推送数据，写操作
		//  data:外部提供的数据和大小
		//  返回false表示失败
		//
		virtual bool onWriteData(char* data, size_t size);
	protected:	
		FFL::IOReader* mReader;
		FFL::IOWriter* mWriter;
	};	

	
}

#endif