/**
  ******************************************************************************
  * @file           : TcpServer.h
  * @author         : xy
  * @brief          : 方便用户
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_TCPSERVER_H_
#define MINIREDIS_SRC_NET_TCPSERVER_H_

class Acceptor;
class EventLoop;

#include "public.h"
#include "InetAddress.h"
#include <memory>
#include <atomic>
#include <map>

class TcpServer {
  using ConnectionMap = std::map<int, TcpConnectionPtr>;
 public:
  TcpServer(std::shared_ptr<EventLoop> loop, const InetAddress &listenAddr);
  ~TcpServer();

  void start();

  std::shared_ptr<EventLoop> getLoop() const { return loop_; }

  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

 private:
  void newConnection(int sockfd, const InetAddress &peerAddr);
  void removeConnection(const TcpConnectionPtr &conn);
 private:
  std::shared_ptr<EventLoop> loop_;
  std::unique_ptr<Acceptor> acceptor_;
  ConnectionMap connections_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;

  std::atomic<int> curConnId_;
};

#endif //MINIREDIS_SRC_NET_TCPSERVER_H_
