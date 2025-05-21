/**
  ******************************************************************************
  * @file           : Channel.h
  * @author         : xy
  * @brief          : 一个 Channel 就是 一个 fd，记录 fd 相关信息
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_CHANNEL_H_
#define MINIREDIS_SRC_NET_CHANNEL_H_

#include "public.h"
class EventLoop;
class Channel {
 public:
  Channel(std::shared_ptr<EventLoop> loop,int fd);

  void handleEvent();

  void setRevents(int revents) { revents_ = revents; }

  int fd() { return fd_; }

  int events() const { return events_; }

  int connStatus() { return connStatus_; }

  void set_connStatus(int status) { connStatus_ = status; }

  bool isNoneEvent() const { return events_ == kNoneEvent; }

  void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

  void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }

  void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

  void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

  void enableReading() {
	  events_ |= kReadEvent;
	  update();
  }

  void disableReading() {
	  events_ &= ~kReadEvent;
	  update();
  }

  void enableWriting() {
	  events_ |= kWriteEvent;
	  update();
  }

  void disableWriting() {
	  events_ &= ~kWriteEvent;
	  update();
  }

  void disableAll() {
	  events_ = kNoneEvent;
	  update();
  }

  bool isWriting() const { return events_ & kWriteEvent; }

  bool isReading() const { return events_ & kReadEvent; }

  std::shared_ptr<EventLoop> ownerLoop() { return loop_; }

  void  remove();

 private:
  void update();
  void handleEventWithGuard();
 private:
  std::shared_ptr<EventLoop> loop_;
  int fd_;
  int revents_;
  int events_;
  int connStatus_;

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};

#endif //MINIREDIS_SRC_NET_CHANNEL_H_
