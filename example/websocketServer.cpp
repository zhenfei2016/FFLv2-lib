
#include <FFL_Netlib.hpp>
#include <net/websocket/FFL_WebSocketServer.hpp>

int FFL_main() {	
	FFL_socketInit();
	
	FFL::WebSocketServer webServer("127.0.0.1",8800,NULL);	
	webServer.start(new FFL::ModuleThread("websocket"));

	FFL::ConsoleLoop console;
	//console.registeCommand()
	console.start(NULL);
	console.eventLoop(NULL);
	webServer.stop();

	return 0;
}
