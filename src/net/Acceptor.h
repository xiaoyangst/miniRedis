/**
  ******************************************************************************
  * @file           : Acceptor.h
  * @author         : xy
  * @brief          : 等待客户端建立新的连接
  * @attention      : None
  * @date           : 2025/5/20
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_ACCEPTOR_H_
#define MINIREDIS_SRC_NET_ACCEPTOR_H_

#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include <functional>

class Acceptor {
 public:
  using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

  Acceptor(EventLoop* loop, const InetAddress &listenAddr);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

  bool isListen() const { return isListen_; }

  void listen();

 private:
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool isListen_ = false;
};

#endif //MINIREDIS_SRC_NET_ACCEPTOR_H_
