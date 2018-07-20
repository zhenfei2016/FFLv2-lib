#include <FFL.h>
#include <net/FFL_NetServer.hpp>
#include <net/http/FFL_Http.hpp>

#include <utils/FFL_StringHelper.hpp>
#include <utils/FFL_File.hpp>

class TrackFileReader {
public:
	TrackFileReader() {
#ifdef WIN32
        const char* path="e:\\track-pipelinemsg.log";
#else
        const char* path="/Users/zhufeifei/work/movic/track-pipelinemsg.log";
#endif
		mFile.open(path);
	}

	~TrackFileReader() {
		mFile.close();
	}

	void readJson(uint32_t lineNum, FFL::Vector< FFL::String >&  arr) {
		bool bQuit=false;
		FFL::String line;
		while (!bQuit) {
			uint8_t buf[1024] = {};
			size_t readed = 0;
			if (FFL_OK != mFile.read(buf, 1023, &readed)) {
				bQuit=true;
			}

			uint8_t* start = buf;
			uint8_t* cur= buf;
			uint8_t* end= cur +readed;
			while (cur != end){
				if (*cur == '\r') {
					line.append((const char*)start, cur - start);
					arr.push_back(toJson(line));
					line = "";
					start = cur + 1;

					if (arr.size() >= lineNum) {
						bQuit = true;
					}
				}				
				cur++;
			}

			if (start != end) {
				line.append((const char*)start, end - start);
			}
		}
	}

	FFL::String toJson(const FFL::String& info) {
		FFL::String json;
		FFL::Vector<FFL::String> arr = FFL::StringSplit(info, ",");
		for (uint32_t i = 0; i < arr.size(); i++) {
			FFL::Vector<FFL::String> keyValue = FFL::StringSplit(arr[i],":");
			if (keyValue.size() != 2) {
				continue;
			}

			FFL::String value;
			FFL::formatString(value, "\"%s\":\"%s\"", keyValue[0].c_str(), keyValue[1].c_str());

			if (!json.empty()) {
				json += "," + value;
			}
			else {
				json = value;
			}
		}
		return "{" + json + "}";
	}

	FFL::File  mFile;
};

TrackFileReader  gTrackFile;
class HttpClient : public FFL::HttpConnectHandler {
public:
	HttpClient() {

	}

	virtual void onReceiveRequest(FFL::HttpConnect* conn, FFL::HttpRequest* request) {
		
		FFL::String json;
		FFL::Vector< FFL::String >  arr;
		gTrackFile.readJson(10,arr);

		for (uint32_t i = 0; i < arr.size(); i++) {
			if (json.empty()) {
				json += arr[i];
			}
			else {
				json += "," + arr[i];
			}
		}		

		FFL::sp<FFL::HttpResponse> res = request->createResponse();
		res->writeJson("["+ json + "]");

		conn->close();
	}	
};




int serverMain() {
	FFL::HttpConnectMgr mgr;
	mgr.setHandler(new HttpClient());

	char buf[1024] = {};
	FFL_socketLocalAddr(buf, 1023);

	FFL::TcpServer server(NULL,5000);
	server.setConnectManager(&mgr);
	server.start();

	while (1) {
		FFL_sleep(1000);
	}

	return 0;
}
