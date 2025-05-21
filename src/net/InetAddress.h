/**
  ******************************************************************************
  * @file           : InetAddress.h
  * @author         : xy
  * @brief          : 封装了 sockaddr_in 结构，提供了基本的 IP 地址与端口操作接口
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_INETADDRESS_H_
#define MINIREDIS_SRC_NET_INETADDRESS_H_

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

class InetAddress {
 public:
  InetAddress(std::string ip, uint16_t port);

  InetAddress() {}

  explicit InetAddress(const struct sockaddr_in &addr)
	  : addr_(addr) {}

  const struct sockaddr_in &getSockAddrInet() const {
	  return addr_;
  }

  void setSockAddrInet(const struct sockaddr_in &addr) {
	  addr_ = addr;
  }

  std::string toIp() const;
  std::string toIpPort() const;
  uint16_t toPort() const;
 private:
  struct sockaddr_in addr_;
};

#endif //MINIREDIS_SRC_NET_INETADDRESS_H_
