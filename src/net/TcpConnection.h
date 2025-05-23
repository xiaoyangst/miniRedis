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

class Channel;
class EventLoop;
class Socket;
#include "public.h"
#include "Buffer.h"
#include "InetAddress.h"
#include <atomic>
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:

  TcpConnection(EventLoop *loop,
				int id,
				int sockfd,
				const InetAddress &localAddr,
				const InetAddress &peerAddr);

  ~TcpConnection();

  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
	  highWaterMarkCallback_ = cb;
	  highWaterMark_ = highWaterMark;
  }

  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

  void connectEstablished();
  void connectDestroyed();

  int id() { return id_; }

  EventLoop *getLoop() const { return loop_; }

  const InetAddress &localAddress() const { return localAddr_; }

  const InetAddress &peerAddress() const { return peerAddr_; }

  bool connected() const { return state_ == kConnected; }

  // 发送数据
  void send(const std::string &buf);
  // 关闭连接
  void shutdown();

 private:
  int id_;

  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };

  void setState(StateE state) { state_ = state; }

  void handleRead();
  void handleWrite();
  void handleClose();
  void handleError();

  void sendInLoop(const void *message, size_t len);
  void shutdownInLoop();

  std::atomic<int> state_;
  bool reading_;
  EventLoop *loop_;

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

  Buffer inputBuffer_;  // 接收数据的缓冲区
  Buffer outputBuffer_; // 发送数据的缓冲区
};

#endif //MINIREDIS_SRC_NET_TCPCONNECTION_H_
