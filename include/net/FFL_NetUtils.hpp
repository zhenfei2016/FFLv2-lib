#ifndef _FFL_NET_UTILS_HPP_
#define _FFL_NET_UTILS_HPP_

#include <utils/FFL_String.hpp>
namespace FFL {
	//
	//  分解url中的host和port
	//  host:port
	void FFL_parseHostport(const String& url, String& host, int32_t& port);
	//
	//  是否一个ip, 是的话返回FFL_Ok
	//
    status_t FFL_isIp(const String& ip);
}

#endif

