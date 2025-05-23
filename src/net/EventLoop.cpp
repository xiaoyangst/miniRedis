/**
  ******************************************************************************
  * @file           : EventLoop.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/20
  ******************************************************************************
  */

#include "EventLoop.h"
#include "Epoller.h"
#include "Logger.h"

EventLoop::EventLoop()
	: isLooping(true), epoller_(new Epoller(this)) {

}

EventLoop::~EventLoop() {

}

void EventLoop::loop() {

	LOG_INFO("EventLoop Start");

	isLooping.store(true);

	while (isLooping.load()) {
		activeChannels_.clear();
		epoller_->poll(kEpollTimeMs, activeChannels_);

		for (const auto &channel : activeChannels_) {
			channel->handleEvent();    // 执行触发事件的连接对应的回调
		}

		//doPendingFunctors();
	}
}

void EventLoop::quit() {
	isLooping.store(false);
}

void EventLoop::updateChannel(Channel *channel) {
	epoller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
	epoller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
	return epoller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() {    // 主从需要
	std::vector<Functor> functors;
	callingPendingFunctors_.store(true);

	{
		std::lock_guard<std::mutex> lg(mtx_);
		functors.swap(pendingFunctors_);
	}

	for (const Functor &functor : functors) {
		functor();
	}
	callingPendingFunctors_.store(false);
}

void EventLoop::addCB(const EventLoop::Functor &cb) {
	cb();
	//pendingFunctors_.push_back(cb);
}



