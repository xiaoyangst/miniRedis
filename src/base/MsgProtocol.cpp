/**
  ******************************************************************************
  * @file           : MsgProtocol.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#include "MsgProtocol.h"
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>
#include <cstdint>

std::string MsgProtocol::pack(const std::string& msg) {
	uint64_t len = msg.size();
	uint64_t len_net = htobe64(len);  // 主机字节序 -> 大端

	std::string packet;
	packet.resize(8 + msg.size());

	std::memcpy(&packet[0], &len_net, 8);          // 写入8字节长度头
	std::memcpy(&packet[8], msg.data(), msg.size()); // 写入消息体

	return packet;
}

// 注意：网络程序必须保证拆包的消息的长度大于 8，否则会抛出异常
std::string MsgProtocol::unpack(const std::string& msg) {
	if (msg.size() < 8) {
		throw std::runtime_error("Incomplete header (less than 8 bytes)");
	}

	uint64_t len_net = 0;
	std::memcpy(&len_net, msg.data(), 8);
	uint64_t len = be64toh(len_net);  // 大端 -> 主机字节序

	if (msg.size() < 8 + len) {
		throw std::runtime_error("Incomplete message body");
	}

	return msg.substr(8, len);
}