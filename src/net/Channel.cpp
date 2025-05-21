/**
  ******************************************************************************
  * @file           : Channel.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#include <bits/poll.h>
#include <iostream>
#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(std::shared_ptr<EventLoop> loop, int fd)
	: loop_(loop), fd_(fd), events_(0), revents_(0), connStatus_(-1) {

}

void Channel::handleEvent() {
	handleEventWithGuard();
}

void Channel::handleEventWithGuard() {
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & POLLNVAL) {
		std::cerr << "fd = " << fd_ << " Channel::handle_event() POLLNVAL\n";
	}

	if (revents_ & (POLLERR | POLLNVAL)) {
		if (errorCallback_) errorCallback_();
	}
	if (revents_ & (POLLIN | POLLPRI)) {
		if (readCallback_) readCallback_();
	}
	if (revents_ & POLLOUT) {
		if (writeCallback_) writeCallback_();
	}
}

void Channel::update() {
	loop_->removeChannel(this);
}

void Channel::remove() {
	loop_->updateChannel(this);
}

