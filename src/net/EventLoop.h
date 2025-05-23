/**
  ******************************************************************************
  * @file           : EventLoop.h
  * @author         : xy
  * @brief          : 事件循环（绝对核心）
  * @attention      : None
  * @date           : 2025/5/20
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_EVENTLOOP_H_
#define MINIREDIS_SRC_NET_EVENTLOOP_H_

#include "Channel.h"

#include <functional>
#include <atomic>
#include <mutex>
#include <vector>

class Epoller;

class EventLoop {
  using ChannelList = std::vector<Channel *>;
 public:
  using Functor = std::function<void()>;
  void loop();
  void quit();

  EventLoop();
  ~EventLoop();

  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);
  bool hasChannel(Channel *channel);
  void addCB(const Functor &cb);
 private:
  void doPendingFunctors();
 private:

  std::atomic<bool> isLooping;
  std::atomic<bool> callingPendingFunctors_;
  std::unique_ptr<Epoller> epoller_;
  std::vector<Functor> pendingFunctors_;
  ChannelList activeChannels_;
  std::mutex mtx_;
};

#endif //MINIREDIS_SRC_NET_EVENTLOOP_H_
