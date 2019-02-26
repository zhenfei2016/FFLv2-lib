
#include <FFL_Netlib.hpp>
#include <net/websocket/FFL_WebSocketClient.hpp>

int FFL_main() {
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	FFL_socketInit();
	
	FFL::sp<FFL::WebSocketClient> client=new  FFL::WebSocketClient("121.40.165.18",8800);
	client->handshark();

	while (1) {
		FFL_sleep(2000);
	}

	return 0;
}
