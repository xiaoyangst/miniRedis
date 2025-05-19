/**
  ******************************************************************************
  * @file           : InetAddress.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#include <cstring>
#include "InetAddress.h"

InetAddress::InetAddress(std::string ip, uint16_t port) {
	std::memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}

std::string InetAddress::toIp() const {
	char buf[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
	return std::string(buf);
}

std::string InetAddress::toIpPort() const {
	return toIp() + ":" + std::to_string(toPort());
}

uint16_t InetAddress::toPort() const {
	return ntohs(addr_.sin_port);
}
