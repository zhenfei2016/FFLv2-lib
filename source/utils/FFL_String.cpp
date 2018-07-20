#include <utils/FFL_String.hpp>

namespace FFL{		
	static const int32_t MAX_LOG_LENGTH = 4096;
	int64_t String2int64_t(String& str) {
		return 0;
	}
	int32_t String2int32_t(String& str) {
		return atoi(str.c_str());
	}
	//
	//  ¸ñÊ½»¯×Ö·û´®
	//
	void formatString(String& str, const char *fmt, ...){	
		char buf[MAX_LOG_LENGTH] = "";
		va_list args;
		va_start(args, fmt);				
		vsnprintf(buf, MAX_LOG_LENGTH - 1, fmt, args);
		va_end(args);

		str.clear();
		str.append(buf);
	}
	//
	//  ·Ö¸î×Ö·û´®
	//
	Vector<String> StringSplit(const String& str, const String& flag){
		Vector<String> arr;

		size_t pos;
		String s = str;
		while ((pos = s.find(flag)) != String::npos) {
			if (pos != 0) {
				arr.push_back(s.substr(0, pos));
			}
			s = s.substr(pos + flag.length());
		}

		if (!s.empty()) {
			arr.push_back(s);
		}

		return arr;
	}

	String stringTrimLeft(const String& str, uint8_t ch) {
		String ret = str;
		while (!ret.empty() && ret.at(0) == ch) {
			ret.erase(ret.begin());
		}
		return ret;
	}
	String stringTrimRight(const String& str, uint8_t ch){
		String ret = str;
		while (!ret.empty() && ret.at(ret.length() - 1) == ch) {
			ret.erase(ret.end() - 1);
		}
		return ret;
	}

	bool stringStartsWith(String& str, const String& prefix) {
		return str.find(prefix) == 0;
	}

	bool stringEndsWith(String& str, const String& subfix) {
		return str.find(subfix) == str.size() - subfix.size();
	}
}

