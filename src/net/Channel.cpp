/**
  ******************************************************************************
  * @file           : Channel.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/19
  ******************************************************************************
  */

#include <iostream>
#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


Channel::Channel(EventLoop* loop, int fd)
	: loop_(loop), fd_(fd), events_(0), revents_(0), connStatus_(kNew) {

}

void Channel::handleEvent() {
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & EPOLLERR) {
		if (errorCallback_) errorCallback_();
	}

	if (revents_ & (EPOLLIN | EPOLLPRI)) {
		if (readCallback_) readCallback_();
	}

	if (revents_ & EPOLLOUT) {
		if (writeCallback_) writeCallback_();
	}
}


void Channel::update() {
	loop_->updateChannel(this);
}

void Channel::remove() {
	loop_->removeChannel(this);
}

