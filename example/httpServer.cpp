
#include <FFL_NetLib.hpp>


class HttpApiLoginHandelr : public FFL::HttpServer::Callback {
public:
	virtual bool onHttpQuery(FFL::HttpRequest* request) {

		FFL::HttpUrl url;
		request->getUrl(url);
		/*		if (url.mQueryParams.size()>0) {
					FFL::sp<FFL::HttpResponse> response = request->makeResponse();
					response->setStatusCode(200);

					char* content = "hi world!";
					int32_t len = strlen(content);

					FFL::HttpHeader header;
					header.setContentLength(len);
					response->setHeader(header);
					response->setContent(content, len);
					response->send();
				}
				else */ {

			FFL::HttpResponseBuilder builder;
			FFL::sp<FFL::HttpResponseFile> response =builder.createResponseFile(request);

			FFL::File file;
			file.open(FFL::String("d://test.txt"));	

			FFL::HttpHeader header;
			header.setContentLength(file.getSize());
			response->setHeader(header);
			response->guessFileType("test.txt");
			response->setReader(&file);
			response->send();			
		}		
		//
		//  关闭这条连接
		//
		return false;
	};
};

class TcpHandler : public FFL::TcpServer::Callback {
public:
	//
	//  aliveTimeUs:保活时长，如果超过这么长时间还没有数据则干掉这个client
	//              <0 一直存活， 
	//
	virtual bool onClientCreate(FFL::TcpClient* client, int64_t* aliveTimeUs) {
		FFL_LOG_DEBUG("TcpHandler:onClientCreate (%p)  ", client);
		return true;
	}
	virtual void onClientDestroy(FFL::TcpClient* client, int reason) {
		FFL_LOG_DEBUG("TcpHandler:onClientDestroy (%p)  %s ", client);
	}
	virtual bool onClientReceived(FFL::TcpClient* client) {		
		size_t readed = 0;
		char buf[4096] = {};
		if (client->read((uint8_t*)buf, 4095, &readed)!=FFL_OK) {
			return false;
		}
		FFL_LOG_DEBUG("TcpHandler:onClientReceived (%p) (len=%d)  %s ",client,readed,buf);
		return true;
	}
};
int FFL_main() {
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	if (1) {
		TcpHandler handler;
		FFL::TcpServer tcpServer("127.0.0.1",5000,&handler);
		tcpServer.start(NULL);
		int32_t waitMs = 5000;
		while (tcpServer.eventLoop(&waitMs)) {
		}
	}
	else {
		FFL::sp<HttpApiLoginHandelr> handler = new HttpApiLoginHandelr();
		FFL::HttpServer httpServer("127.0.0.1", 5000);

		FFL::HttpServer::HttpApiKey key;
		key.mPath = "/FFLv2";
		key.mQuery = "login";
		httpServer.registerApi(key,handler);
		httpServer.start(NULL);
		int32_t waitMs = 5000;
		while (httpServer.eventLoop(&waitMs)) {
		}
	}
	
	
	//FFL::HttpConnectMgr mgr;
	//mgr.registerApi("/FFLv2?login",handler);	
	//FFL::TcpServer server(NULL,5000);
	//server.setConnectManager(&mgr);
	//server.start();



	return 0;
}
