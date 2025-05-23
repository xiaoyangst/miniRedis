/**
  ******************************************************************************
  * @file           : Epoller.h
  * @author         : xy
  * @brief          : 对 epoll 封装
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_NET_EPOLLER_H_
#define MINIREDIS_SRC_NET_EPOLLER_H_

#include <sys/epoll.h>
#include "public.h"
#include "Channel.h"
#include "EventLoop.h"

class Epoller {
  using EventList = std::vector<struct epoll_event>;
 public:
  using ChannelList = std::vector<Channel *>;
  using ChannelMap = std::unordered_map<int, Channel *>;
 public:
  explicit Epoller(EventLoop *loop);
  ~Epoller();

  void poll(int timeoutMs, ChannelList &activeChannels);    //TODO 后期加入定时器，poll 函数返回值得是有事件发生的时间
  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);
  bool hasChannel(Channel *channel) const;
 private:

  void fillActiveChannels(int numEvents,
						  ChannelList &activeChannels) const;
  void update(int operation, Channel *channel);

  std::string operationToString(int op);
 public:
  ChannelMap channels_;
 private:
  EventLoop *ownerLoop_;
  int epollFd_;
  EventList events_;
};

#endif //MINIREDIS_SRC_NET_EPOLLER_H_
