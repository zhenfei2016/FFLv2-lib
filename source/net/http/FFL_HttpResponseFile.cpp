/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponseFile.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/28
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http应答文件格式
*/

#include <net/http/FFL_HttpResponseFile.hpp>
#include <FFL_File.hpp>
#include <net/http/FFL_HttpClient.hpp>

namespace FFL {
	
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
            { ".js.map","text/javascript.map" },
			{ ".css","text/css" },
            { ".css.map","text/css.map" },
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
			if (filePath.endWith(sMap[i].ext)) {
				fileType = sMap[i].contentType;
				return true;
			}
		}

		return false;		
	}

	////
	////  应答这个文件，返回是否有效的应答了
	////
	//bool HttpFileResponse::response(const char* path) {
	//	String contentType;
	//	if (!isSupportFile(path, contentType)) {
	//		return false;
	//	}		

	//	String content;
	//	if (!readFileContent(path, content)) {
	//		return false;
	//	}


	//	String format;
	//	format = "HTTP/1.1 200 OK \r\n"
	//		"Content-Type:%s;charset=utf-8\r\n"
	//		"Content-Length:%d\r\n\r\n";

	//	String data;
	//	formatString(data, format.c_str(), contentType.c_str(),content.size());		

	//	FFL::IOWriter* writer = mConn->getWriter();
	//	size_t nWrited = 0;
	//	writer->write((void*)data.c_str(), data.size(), &nWrited);
	//	writer->write((void*)content.c_str(), content.size(), &nWrited);

	//	mConn->close();
	//	return true;
	//}

	HttpResponseFile::HttpResponseFile(FFL::sp<HttpClient> client):HttpResponse(client),
		mReader(NULL),mWriter(NULL){
		
	}

	HttpResponseFile::~HttpResponseFile() {

	}

	bool HttpResponseFile::setReader(FFL::IOReader* reader) {
		if (mWriter != NULL) {
			return false;
		}
		mReader = reader;
		return true;
	}
	bool HttpResponseFile::guessFileType(const char* fileName) {
		String contentType;
		if (isSupportFile(String(fileName), contentType)) {
			mHeader.setContentType(contentType);
			return true;
		}

		return false;
	}
	bool HttpResponseFile::setWriter(FFL::IOWriter* writer, uint32_t size) {
		if (mReader != NULL) {
			return false;
		}
		mWriter = writer;
		mHeader.setContentLength(size);
		return true;
	}
	bool HttpResponseFile::writeContent() {
		return sendStream(this);
	}
	/*   HttpStreamCallback  */
	//
	//  拉取数据，读操作
	//  data:外部提供的缓冲区
	//  bufLen:外部提供的缓冲区大小
	//  size:实质上填充了多少
	//  返回false表示失败
	//
	bool HttpResponseFile::onReadData(char* data, size_t bufLen, size_t* size) {
		if (!mReader) {
			return false;
		}

		return FFL_OK == mReader->read((uint8_t*)data, bufLen, size);
	}
	//
	//  推送数据，写操作
	//  data:外部提供的数据和大小
	//  返回false表示失败
	//
	bool HttpResponseFile::onWriteData(char* data, size_t size) {
		if (!mWriter) {
			return false;
		}
		
		return FFL_OK == mWriter->write(data, size,NULL);
	}
}
