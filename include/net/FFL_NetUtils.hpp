#ifndef _FFL_NET_UTILS_HPP_
#define _FFL_NET_UTILS_HPP_

#include <utils/FFL_String.hpp>
namespace FFL {
	//
	//  �ֽ�url�е�host��port
	//  host:port
	void FFL_parseHostport(const String& url, String& host, int32_t& port);
	//
	//  �Ƿ�һ��ip, �ǵĻ�����FFL_Ok
	//
    status_t FFL_isIp(const String& ip);
}

#endif

