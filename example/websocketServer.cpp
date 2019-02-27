
#include <FFL_Netlib.hpp>
#include <net/websocket/FFL_WebSocketServer.hpp>

int FFL_main() {
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	FFL_socketInit();
	
	FFL::WebSocketServer webServer("127.0.0.1",8800,NULL);	
	webServer.start(NULL);
	while (1) {
		webServer.eventLoop(NULL);
		FFL_sleep(500);
	}

	return 0;
}
