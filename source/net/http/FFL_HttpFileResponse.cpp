/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpFileResponse.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/28
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http应答文件格式
*/

#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpConnect.hpp>
#include <utils/FFL_File.hpp>
#include <utils/FFL_StringHelper.hpp>
namespace FFL {
	HttpFileResponse::HttpFileResponse(HttpConnect* conn) :HttpResponse(conn) {
	}
	HttpFileResponse::~HttpFileResponse(){
	}
	//
	//  是否支持这文件
	//
	struct ContentTypeMap{
		const char* ext;
		const char* contentType;
	};

	static bool isSupportFile(String filePath,String& fileType) {		
		static ContentTypeMap  sMap[] = {
			{".html","text/html" },
			{ ".js","text/javascript" },
			{ ".css","text/css" },
			{ ".json","text/json" },
			{ ".xml","text/xml" },
			{ ".rss","application / rss + xml" },
			{ ".ico","image/x-icon" },
			{ ".png","image/png" },
			{ ".jpeg","image/jpeg" },
			{ ".jpg","image/jpeg" },
			{ ".gif","image/gif" }
		};

		for (int32_t i = 0; i < FFL_ARRAY_ELEMS(sMap); i++) {
			if (stringEndsWith(filePath, sMap[i].ext)) {
				fileType = sMap[i].contentType;
				return true;
			}
		}

		return false;		
	}

	static bool readFileContent(const char* path,String& content) {
		FFL::File file;
		if (FFL_OK != file.open(path)) {
			return false;
		}

		uint8_t buf[4096] = {};
		while (true) {
			size_t size = 0;			
			if (FFL_OK != file.read(buf, 4096, &size)) {
				break;
			}
			if (size == 0) {
				break;
			}

			content.append((const char*)buf, size);
		}

		file.close();
		return true;
	}

	//
	//  应答这个文件，返回是否有效的应答了
	//
	bool HttpFileResponse::response(const char* path) {
		String contentType;
		if (!isSupportFile(path, contentType)) {
			return false;
		}		

		String content;
		if (!readFileContent(path, content)) {
			return false;
		}


		String format;
		format = "HTTP/1.1 200 OK \r\n"
			"Content-Type:%s;charset=utf-8\r\n"
			"Content-Length:%d\r\n\r\n";

		String data;
		formatString(data, format.c_str(), contentType.c_str(),content.size());		

		FFL::IOWriter* writer = mConn->getWriter();
		size_t nWrited = 0;
		writer->write((void*)data.c_str(), data.size(), &nWrited);
		writer->write((void*)content.c_str(), content.size(), &nWrited);

		mConn->close();
		return true;
	}
}
