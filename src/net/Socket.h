/**
  ******************************************************************************
  * @file           : Socket.h
  * @author         : xy
  * @brief          : 不是创建 socket_fd，而是对 socket_fd 进行属性设置
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_SOCKET_H_
#define MINIREDIS_SRC_NET_SOCKET_H_

#include "InetAddress.h"

class Socket {
 public:
  explicit Socket(int sockfd);

  ~Socket();

  int fd() const { return sockfd_; }

  void bindAddress(const InetAddress &localaddr);
  void connect(const InetAddress& serverAddr);
  void listen();
  int accept(InetAddress *peeraddr);

  void shutdownWrite();
  void setTcpNoDelay(bool on);
  void setNonBlocking(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);


 private:
  const int sockfd_;
};

#endif //MINIREDIS_SRC_NET_SOCKET_H_
