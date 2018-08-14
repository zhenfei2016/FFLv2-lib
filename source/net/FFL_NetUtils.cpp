

#include <utils/FFL_StringHelper.hpp>

namespace FFL {

	void FFL_parseHostport(const String& url, String& host, int32_t& port){
		size_t pos = url.find(":");
		if (pos != std::string::npos) {
			String p = url.substr(pos + 1);
			host = url.substr(0, pos);
			port = ::atoi(p.c_str());
		}else {
			host = url;
		}
	}

	//
	//  是否一个ip, 是的话返回FFL_Ok
	//
	status_t FFL_isIp(const String& ip) {
		return ip.size() > 0 ?FFL_OK:FFL_FAILED;
	}
}