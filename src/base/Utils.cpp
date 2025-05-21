/**
  ******************************************************************************
  * @file           : Utils.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#include "Utils.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cerrno>

struct sockaddr_in Utils::getLocalAddr(int sockfd) {
	struct sockaddr_in localaddr;
	std::memset(&localaddr, 0, sizeof(localaddr));
	socklen_t addrLen = static_cast<socklen_t>(sizeof(localaddr));

	if (::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localaddr), &addrLen) < 0) {
		std::cerr << "Utils::getLocalAddr error: " << std::strerror(errno) << std::endl;
		std::memset(&localaddr, 0, sizeof(localaddr));  // 防止未初始化返回
	}

	return localaddr;
}

struct sockaddr_in Utils::getPeerAddr(int sockfd) {
	struct sockaddr_in peeraddr;
	std::memset(&peeraddr, 0, sizeof(peeraddr));
	socklen_t addrLen = static_cast<socklen_t>(sizeof(peeraddr));

	if (::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&peeraddr), &addrLen) < 0) {
		std::cerr << "Utils::getPeerAddr error: " << std::strerror(errno) << std::endl;
		std::memset(&peeraddr, 0, sizeof(peeraddr));  // 防止未初始化返回
	}

	return peeraddr;
}