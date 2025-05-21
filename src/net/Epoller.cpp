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
#include "Epoller.h"

Epoller::Epoller(std::shared_ptr<EventLoop> loop)    // 创建 epoll_fd
	: ownerLoop_(std::move(loop))
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
void Epoller::updateChannel(std::shared_ptr<Channel> channel) {
	// 先看状态
	auto connStatus = channel->connStatus();

	if (connStatus == kNew || connStatus == kDeleted) {        // 对不存在的 channel 的处理
		auto fd = channel->fd();
		if (connStatus == kNew) {
			channels_[fd] = channel;
		}
		// kDeleted 状态下，表面之前已经不关心任何事件，现在又有感兴趣的时间了
		channel->set_connStatus(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} else {    // 对已存在的 channel 的处理
		int fd = channel->fd();
		(void)fd;
		if (channel->isNoneEvent()) {    // 不关注任何事件，删除
			update(EPOLL_CTL_DEL, channel);
			channel->set_connStatus(kDeleted);
		} else {                       // 只要还要关注事件，就添加
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

// 移除
void Epoller::removeChannel(std::shared_ptr<Channel> channel) {
	auto fd = channel->fd();
	auto connStatus = channel->connStatus();
	channels_.erase(fd);

	if (connStatus == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_connStatus(kNew);
}

// 把发生事件的 Channel 加入到 activeChannels
void Epoller::fillActiveChannels(int numEvents, ChannelList &activeChannels) const {
	for (int i = 0; i < numEvents; ++i) {
		auto channel = std::make_shared<Channel>(ownerLoop_, events_[i].data.fd);
		channel->setRevents(events_[i].events);    // 记录 fd 发生的事件类型
		activeChannels.push_back(channel);
	}
}

void Epoller::update(int operation, std::shared_ptr<Channel> channel) {
	struct epoll_event event;
	memset(&event, 0, sizeof(event));

	event.events = channel->events();
	event.data.ptr = channel.get();    //TODO 后面测试，看是否还是全部用 裸指针
	int fd = channel->fd();

	if (::epoll_ctl(epollFd_, operation, fd, &event) < 0) {
		if (operation == EPOLL_CTL_DEL) {
			std::cerr << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
		} else {
			std::cerr << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
		}
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

bool Epoller::hasChannel(std::shared_ptr<Channel> channel) const {
	const auto it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}
