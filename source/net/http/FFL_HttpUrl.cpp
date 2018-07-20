/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_HttpUrl.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/18
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  分解URL
*/

#include <net/http/FFL_HttpUrl.hpp>
#include "http-parser-2.1/http_parser.h"

namespace FFL {	
	HttpUrl::HttpUrl() {
		mPort = 0;
	}
	HttpUrl::~HttpUrl() {
	}

	static void getUrlField(const String& url, String& val, http_parser_url* parserUrl, http_parser_url_fields field)
	{
		if ((parserUrl->field_set & (1 << field)) == 0) {
			//
			//  是否有这个field
			//
			val= "";		
		}
		else {

			int32_t off = parserUrl->field_data[field].off;
			int32_t len = parserUrl->field_data[field].len;
			val = url.substr(off, len);
		}
	}
	//
	// 输入类型： http://www.xxx.com:8000/v1/api
	//            mSchema:mHost mPort mPath mQuery
	bool HttpUrl::parse(const String &url){		
		mPort = 0;
		FFL_LOG_DEBUG("HttpUrl::parse %s",url.c_str());
		http_parser_url parserUrl;
		if (http_parser_parse_url(url.c_str(), url.length(), 0, &parserUrl)!= 0) {			
			return false;
		}

		getUrlField(url,mSchema, &parserUrl, UF_SCHEMA);
		getUrlField(url, mHost, &parserUrl, UF_HOST);		
		mPort=parserUrl.port;
		getUrlField(url, mPath, &parserUrl, UF_PATH);
		getUrlField(url, mQuery, &parserUrl, UF_QUERY);

		FFL_LOG_DEBUG("HttpUrl::parse schema=%s  host=%s port=%d path=%s query=%s",
			mSchema.c_str(),
			mHost.c_str(),
			mPort,
			mPath.c_str(),
			mQuery.c_str());
		return true;
	}	
}