#include <FFL.h>
#include <net/FFL_NetEventLoop.hpp>
#include <net/FFL_NetTcpServer.hpp>

static bool gQuit = false;
class UdpHandler : public FFL::NetEventHandler {
public:
	//
	//  返回是否还继续读写
	//  priv:透传数据
	//
	virtual bool onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv) {
		char buf[4096] = {};
		size_t readed = 0;
		char srcIp[32] = {};
		uint16_t srcPort = 0;
		if (readable) {			
			if (FFL_OK == FFL_socketReadFrom(fd, buf, 4096, &readed, srcIp, &srcPort)) {
				FFL_LOG_DEBUG("udp fd:%d   recv:%s", fd, buf);

				if (buf[0] == 'q'&& buf[1] == 0) {
					gQuit = true;
				}
				int i = 0;
				i++  ;
				return true;
			}
		}
		return true;
	}
};


class TcpHandler : public FFL::NetEventHandler {
public:
	//
	//  返回是否还继续读写
	//  priv:透传数据
	//
	virtual bool onNetEvent(NetFD fd, bool readable, bool writeable, bool exception, void* priv) {
		char buf[4096] = {};
		size_t readed = 0;
		
		int16_t srcPort = 0;
		if (readable) {
			if (FFL_OK == FFL_socketRead(fd, buf, 4096, &readed)) {
				FFL_LOG_DEBUG("tcp fd:%d   recv:%s", fd, buf);
				int i = 0;
				i++;
				return true;
			}
			else {
				return false;
			}
		}
		return true;
	}
};

class MyTcpListener : public FFL::TcpServerListener {
public:
	virtual void onAcceptClient(NetFD fd) {
		FFL_LOG_DEBUG("add tcp fd=%d",fd);
		mEventLoopp->addFd(fd, mHandler, NULL);
	}


	FFL::NetEventLoop* mEventLoopp;
	TcpHandler* mHandler;
};

int FFL_main() {
	char exePath[1024] = {};
	char exeName[1024] = {};
	FFL_getCurrentProcessPath(exePath, 1023, exeName);

	FFL_socketInit();

	FFL::NetEventLoop evnetLoop(-1);

	UdpHandler handler;
	TcpHandler tcpHandler;

	NetFD fd1=0;
	if (FFL_OK == FFL_socketAnyAddrUdpServer(5000, &fd1)) {
		evnetLoop.addFd(fd1, &handler, NULL);
	}

	fd1 = 0;
	if (FFL_OK == FFL_socketAnyAddrUdpServer(5004, &fd1)) {
		evnetLoop.addFd(fd1, &handler, NULL);
	}


	MyTcpListener tcpListener;
	tcpListener.mEventLoopp = &evnetLoop;
	tcpListener.mHandler = &tcpHandler;
	FFL::NetTcpServer tcpServer("127.0.0.1",5000,&tcpListener);
	tcpServer.start(new FFL::ModuleThread("TcpListen"));	

	evnetLoop.start(new FFL::ModuleThread("udpEvent"));

	while (!gQuit) {
		FFL_sleep(1000);
	}

	tcpServer.stop();
	FFL_LOG_DEBUG("evnetLoop stop");
	evnetLoop.stop();
	FFL_LOG_DEBUG("evnetLoop stop ok");

	return 0;
}
