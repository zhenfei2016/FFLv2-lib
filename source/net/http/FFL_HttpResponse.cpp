/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpResponse.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/14
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  http应答
*/

#include <net/http/FFL_HttpResponse.hpp>
#include <net/http/FFL_HttpRequest.hpp>
#include <net/http/FFL_HttpConnect.hpp>
#include <utils/FFL_File.hpp>
#include <utils/FFL_StringHelper.hpp>
namespace FFL {
	const char* JSON_HEADER = "HTTP/1.1 200 OK \r\n"
		"Content-Type: application/json;charset=utf-8\r\n";

	HttpResponse::HttpResponse(HttpConnect* conn):mConn(conn){
	}
	HttpResponse::~HttpResponse() {

	}

	//
	//  设置返回的状态码
	//
	void HttpResponse::setStatusCode(int32_t code) {
		mStatusCode = code;
	}	

   
	bool HttpResponse::writeHeader(HttpHeader* header, const char* content) {
		return true;
	}

	bool HttpResponse::writeJson(const String& json) {
		String header;
		{
			String format;
			format = "HTTP/1.1 200 OK \r\n"
				"Content-Type: application/json;charset=utf-8\r\n"
				"Access-Control-Allow-Origin:*\r\n"
				"Access-Control-Allow-Methods:GET, POST, PUT, DELETE\r\n"
				"Access-Control-Allow-Headers:x-custom\r\n"
				"Access-Control-Allow-Credentials:true\r\n"
				"Content-Length:%d\r\n\r\n";			
			formatString(header, format.c_str(), json.size());			
		}

		int size=header.size();
		String data;
		data = header + json;		

		FFL::IOWriter* writer = mConn->getWriter();
		size_t nWrited = 0;
		const uint8_t* src =(const uint8_t*)data.c_str();
		const uint8_t* end= src + data.size();
		while (src<end){
			size_t w = 0;
			if (FFL_OK != writer->write((void*)src, end- src, &w)) {
				FFL_LOG_ERROR("HttpResponse write failed. ");
				break;
			}
			src += w;
			nWrited += w;
		}		
		return nWrited>0;
	}
    
    void HttpResponse::finish(){
        String content="Not Found";
        
        String format;
        format = "HTTP/1.1 404 OK \r\n"
        "Content-Type: text/plain;charset=utf-8\r\n"
        "Content-Length:%d\r\n\r\n%s";
        
        String data;
        formatString(data, format.c_str(), content.size(), content.c_str());
        

        
        FFL::IOWriter* writer = mConn->getWriter();
        size_t nWrited = 0;
        writer->write((void*)data.c_str(), data.size(), &nWrited);
        mConn->close();
    }
	//
	//  创建应答
	//
	sp<HttpRequest>  HttpResponse::createRequest() {
		HttpRequest* req = new HttpRequest(mConn);
		return req;
	}

	HttpHtmlResponse::HttpHtmlResponse(HttpConnect* conn):HttpResponse(conn){
		mHtmlFile = NULL;
	}
	HttpHtmlResponse::~HttpHtmlResponse() {
		FFL_SafeFree(mHtmlFile);
	}

	void HttpHtmlResponse::htmlPath(const char* path) {
		FFL_SafeFree(mHtmlFile);
		mHtmlFile = new FFL::File();
		mHtmlFile->open(path);
	}
	void HttpHtmlResponse::response() {
		String format;
		format = "HTTP/1.1 200 OK \r\n"
			"Content-Type: text/html;charset=utf-8\r\n"
			"Content-Length:%d\r\n\r\n";

		String content ;
		while (true) {
			size_t size = 0;
			uint8_t buf[1024] = {};
			if (FFL_OK!=mHtmlFile->read(buf, 1024, &size)) {
				break;
			}
			if (size == 0) {
				break;
			}

			content.append((const char*)buf, size);
		}

		String data;
		formatString(data, format.c_str(), content.size());
		data.append(content.c_str(), content.size());

		FFL::IOWriter* writer = mConn->getWriter();
		size_t nWrited = 0;
		writer->write((void*)data.c_str(), data.size(), &nWrited);
		mConn->close();
	}

	HttpFileResponse::HttpFileResponse(HttpConnect* conn) :HttpResponse(conn) {

	}
	HttpFileResponse::~HttpFileResponse(){
	}

	//
	//  是否支持这文件
	//
	static bool isSupportFile(String filePath,String& fileType) {		
		if (stringEndsWith(filePath, ".html")) {
			fileType="text/html";
		}
		else if (stringEndsWith(filePath, ".js")) {
			fileType = "text/javascript";
		}
		else if (stringEndsWith(filePath, ".css")) {
			fileType = "text/css";
		}
		else {
			return false;
		}

		return true;
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
