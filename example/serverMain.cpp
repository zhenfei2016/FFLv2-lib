#include <FFL.h>
#include <net/FFL_NetServer.hpp>
#include <net/http/FFL_Http.hpp>

#include <utils/FFL_StringHelper.hpp>
#include <utils/FFL_File.hpp>

class HttpApiLoginHandelr : public FFL::HttpApiHandler {
public:
	virtual void onHttpQuery(FFL::HttpConnect* conn, FFL::String& path, FFL::String& query) {
		FFL::sp<FFL::HttpResponse> res = conn->createResponse();
		res->writeJson("{\"login\":\"1\"");
		conn->realese();
	};
};

int serverMain() {
	FFL::HttpConnectMgr mgr;
	mgr.registerApi("/FFLv2?login",new HttpApiLoginHandelr());	
	FFL::TcpServer server(NULL,5000);
	server.setConnectManager(&mgr);
	server.start();

	while (1) {
		FFL_sleep(1000);
	}

	return 0;
}
