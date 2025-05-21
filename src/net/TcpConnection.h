/**
  ******************************************************************************
  * @file           : TcpConnection.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_TCPCONNECTION_H_
#define MINIREDIS_SRC_NET_TCPCONNECTION_H_

#include "public.h"
class Channel;
class EventLoop;
class Socket;
#include "InetAddress.h"
class TcpConnection {
 public:

  TcpConnection(std::shared_ptr<EventLoop> loop,
				int sockfd,
				const InetAddress &localAddr,
				const InetAddress &peerAddr);

  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
	  highWaterMarkCallback_ = cb;
	  highWaterMark_ = highWaterMark;
  }

  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

  // called when TcpServer accepts a new connection
  void connectEstablished();   // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed();  // should be called only once

  int id() { return id_; }

 private:
  int id_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  HighWaterMarkCallback highWaterMarkCallback_;
  CloseCallback closeCallback_;
  size_t highWaterMark_;
};

#endif //MINIREDIS_SRC_NET_TCPCONNECTION_H_
