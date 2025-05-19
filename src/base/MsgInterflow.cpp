/**
  ******************************************************************************
  * @file           : MsgInterflow.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#include "public.h"
#include "MsgProtocol.h"
#include "MsgInterflow.h"

bool MsgInterflow::sendAll(int sockfd, const std::string &data) {
	size_t total = 0;
	while (total < data.size()) {
		ssize_t sent = send(sockfd, data.data() + total, data.size() - total, MSG_NOSIGNAL);
		if (sent <= 0) return false;
		total += sent;	// 继续发送没有发送完的数据
	}
	return true;
}

bool MsgInterflow::recvN(int sockfd, std::string &out, size_t n) {
	out.resize(n);
	size_t total = 0;
	while (total < n) {
		ssize_t receive = recv(sockfd, &out[total], n - total, MSG_WAITALL);
		if (receive <= 0) return false;
		total += receive;
	}
	return true;
}

bool MsgInterflow::sendMsg(int sockfd, const std::string &message) {
	std::string packed = MsgProtocol::pack(message);
	return sendAll(sockfd, packed);
}

bool MsgInterflow::recvMsg(int sockfd, std::string &out_message) {
	std::string header;
	if (!recvN(sockfd, header, 8)) {	// 先接收头部指定字节
		return false;
	}

	uint64_t len_net = 0;
	std::memcpy(&len_net, header.data(), 8);
	uint64_t len = be64toh(len_net);	// 得到消息体长度

	if (len > MAX_MSG_LEN) {  // 限制消息大小
		std::cerr << "Message too large\n";
		return false;
	}

	std::string body;
	if (!recvN(sockfd, body, len)) {
		return false;
	}

	out_message = MsgProtocol::unpack(header + body);
	return true;
}