/**
  ******************************************************************************
  * @file           : Epoller.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */


#include <iostream>
#include <cstring>
#include "Logger.h"
#include "Epoller.h"

Epoller::Epoller(EventLoop *loop)    // 创建 epoll_fd
	: ownerLoop_(loop)
	  , epollFd_(::epoll_create1(EPOLL_CLOEXEC))
	  , events_(kInitEventListSize) {
	if (epollFd_ < 0) {
		std::cerr << "epoll_create1 failed\n";
	}
}

void Epoller::poll(int timeoutMs, Epoller::ChannelList &activeChannels) {    // epoll_wait
	auto numEvents = epoll_wait(epollFd_, events_.data(), static_cast<int >(events_.size()), timeoutMs);
	// TODO 完善 epoll_wait 返回值的处理
	if (numEvents > 0) {
		fillActiveChannels(numEvents, activeChannels);
		if (numEvents == static_cast<int >(events_.size())) {    // 说明发生的事件的连接很多，容器都装满了，进行扩容
			events_.resize(events_.size() * 2);
		}
	} else if (numEvents == 0) {
		std::cerr << "nothing happened\n";
	} else {
		std::cerr << "EPollPoller::poll()\n";
	}
}

// 添加和更新
void Epoller::updateChannel(Channel *channel) {

	auto connStatus = channel->connStatus();
	int fd = channel->fd();

	if (connStatus == kNew || connStatus == kDeleted) {
		if (connStatus == kNew) {
			// 新加入的 channel，加入管理 map
			channels_[fd] = channel;
		}
		channel->set_connStatus(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} else {
		// 已经添加的channel
		if (channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->set_connStatus(kDeleted);
			// 记得从channels_中删除channel，避免悬空指针
			channels_.erase(fd);
		} else {
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

// 移除
void Epoller::removeChannel(Channel *channel) {
	auto fd = channel->fd();
	auto connStatus = channel->connStatus();
	channels_.erase(fd);

	if (connStatus == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_connStatus(kNew);
}


void Epoller::fillActiveChannels(int numEvents, ChannelList &activeChannels) const {
	for (int i = 0; i < numEvents; ++i) {
		// 使用 data.ptr 中的指针，得到原始的 Channel 实例
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);	// 容易犯错，记得是从 events_ 中取 Channel
		channel->setRevents(events_[i].events);  // 设置发生的事件类型
		activeChannels.push_back(channel);
	}
}


void Epoller::update(int operation, Channel *channel) {
	struct epoll_event event;
	memset(&event, 0, sizeof(event));

	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();


	LOG_INFO("epoll_ctl op=%s fd=%d events=%d", operationToString(operation).c_str(), fd, event.events);

	if (::epoll_ctl(epollFd_, operation, fd, &event) < 0) {
		std::cerr << "epoll_ctl failed, op=" << operationToString(operation)
				  << ", fd=" << fd
				  << ", errno=" << errno
				  << ", err=" << strerror(errno) << std::endl;
	}
}

std::string Epoller::operationToString(int op) {
	switch (op) {
		case EPOLL_CTL_ADD: return "ADD";
		case EPOLL_CTL_DEL: return "DEL";
		case EPOLL_CTL_MOD: return "MOD";
		default: return "Unknown Operation";
	}
}

Epoller::~Epoller() {
	::close(epollFd_);
}

bool Epoller::hasChannel(Channel *channel) const {
	const auto it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}
