/**
  ******************************************************************************
  * @file           : Socket.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "Socket.h"

Socket::Socket(int sockfd)
	: sockfd_(sockfd) {}

Socket::~Socket() {
	::close(sockfd_);
}

void Socket::setTcpNoDelay(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

void Socket::setReuseAddr(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setKeepAlive(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}

int Socket::accept(InetAddress *peeraddr) {
	struct sockaddr_in addr;
	socklen_t len = sizeof addr;
	int connfd = ::accept(sockfd_, (struct sockaddr *)&addr, &len);
	if (connfd >= 0 && peeraddr) {
		*peeraddr = InetAddress(addr);
	}
	return connfd;
}

void Socket::bindAddress(const InetAddress &localaddr) {
	if (bind(sockfd_, (struct sockaddr *)&localaddr.getSockAddrInet(), sizeof(struct sockaddr_in)) < 0) {
		std::cerr << "Socket::bindAddress() failed\n";
	}
}

void Socket::listen() {
	if (::listen(sockfd_, SOMAXCONN) < 0) {
		std::cerr << "Socket::listen() failed\n";
	}
}

void Socket::shutdownWrite() {
	if (::shutdown(sockfd_, SHUT_WR) < 0) {
		std::cerr << "Socket::shutdownWrite() failed\n";
	}
}

void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
	int optval = on ? 1 : 0;
	if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval) < 0) {
		std::cerr << "Socket::setReusePort() failed\n";
	}
#else
	if (on) {
		std::cerr << "SO_REUSEPORT is not supported\n";
	}
#endif
}

void Socket::setNonBlocking(bool on) {
	int flags = fcntl(sockfd_, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "fcntl(F_GETFL) failed\n";
	}

	if (on) {
		if (fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK) == -1) {
			std::cerr << "fcntl(F_SETFL, O_NONBLOCK) failed\n";
		}
	} else {
		if (fcntl(sockfd_, F_SETFL, flags & ~O_NONBLOCK) == -1) {
			std::cerr << "fcntl(F_SETFL, ~O_NONBLOCK) failed\n";
		}
	}
}

void Socket::connect(const InetAddress &serverAddr) {
	const struct sockaddr_in &addr = serverAddr.getSockAddrInet();
	if (::connect(sockfd_, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cerr << "Socket::connect() failed\n";
	}
}
