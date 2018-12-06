#ifndef _FFL_SOCKET_PAIR_HPP_
#define _FFL_SOCKET_PAIR_HPP_
#include <net/base/FFL_Net.h>

namespace FFL{
	class  SocketPair{		
	public:
		SocketPair();
		~SocketPair();
		
		bool create();
		void destroy();

		NetFD getFd0() const;
		NetFD getFd1() const;

		//
		//  fd0иоп╢
		//
		bool writeFd0(const uint8_t* data,size_t size,size_t* writedSize);
		//
		//  fd1ио╤а
		//
		bool readFd1(uint8_t* data, size_t size, size_t* readedSize);
	private:
		DISABLE_COPY_CONSTRUCTORS(SocketPair);
	private:
		NetFD mFd[2];
	}; 
}

#endif 
