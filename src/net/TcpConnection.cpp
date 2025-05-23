/**
  ******************************************************************************
  * @file           : TcpConnection.cpp
  * @author         : xy
  * @brief          : 代表一个服务器与客户端的连接
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include "Logger.h"
#include "EventLoop.h"

#include <functional>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <string>

TcpConnection::TcpConnection(EventLoop* loop,
							 int id,
							 int sockfd,
							 const InetAddress &localAddr,
							 const InetAddress &peerAddr)
	: loop_(loop), id_(id)
	  , state_(kConnecting)
	  , reading_(true)
	  , socket_(new Socket(sockfd))
	  , channel_(new Channel(loop, sockfd))
	  , localAddr_(localAddr)
	  , peerAddr_(peerAddr)
	  , highWaterMark_(64 * 1024 * 1024) // 64M
{
	// 下面给channel设置相应的回调函数，poller给channel通知感兴趣的事件发生了，channel会回调相应的操作函数

	channel_->setReadCallback(
		[this]() {
		  this->handleRead();
		}
	);

	channel_->setWriteCallback(
		[this]() {
		  this->handleWrite();
		}
	);

	channel_->setCloseCallback(
		[this]() {
		  this->handleClose();
		}
	);

	channel_->setErrorCallback(
		[this]() {
		  this->handleError();
		}
	);

	LOG_INFO("TcpConnection::ctor[%d] at fd=%d\n", id_, sockfd);
}

TcpConnection::~TcpConnection() {
	LOG_INFO("TcpConnection::dtor[%d] at fd=%d state=%d \n",
			 id_, channel_->fd(), (int)state_);
}

void TcpConnection::send(const std::string &buf) {
	if (state_ == kConnected) {
		loop_->addCB(std::bind(
			&TcpConnection::sendInLoop,
			this,
			buf.c_str(),
			buf.size()
		));
	}
}



void TcpConnection::shutdown() {
	if (state_ == kConnected) {
		setState(kDisconnecting);
		loop_->addCB(
			std::bind(&TcpConnection::shutdownInLoop, this)
		);
	}
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;

	// 之前调用过该connection的shutdown，不能再进行发送了
	if (state_ == kDisconnected)
	{
		LOG_ERROR("disconnected, give up writing!");
		return;
	}

	// 表示channel_第一次开始写数据，而且缓冲区没有待发送数据
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
	{
		nwrote = ::write(channel_->fd(), data, len);
		if (nwrote >= 0)
		{
			remaining = len - nwrote;
			if (remaining == 0 && writeCompleteCallback_)
			{
				// 既然在这里数据全部发送完成，就不用再给channel设置epollout事件了
				loop_->addCB(
					std::bind(writeCompleteCallback_, shared_from_this())
				);
			}
		}
		else // nwrote < 0
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_ERROR("TcpConnection::sendInLoop");
				if (errno == EPIPE || errno == ECONNRESET) // SIGPIPE  RESET
				{
					faultError = true;
				}
			}
		}
	}

	// 说明当前这一次write，并没有把数据全部发送出去，剩余的数据需要保存到缓冲区当中，然后给channel
	// 注册epollout事件，poller发现tcp的发送缓冲区有空间，会通知相应的sock-channel，调用writeCallback_回调方法
	// 也就是调用TcpConnection::handleWrite方法，把发送缓冲区中的数据全部发送完成
	if (!faultError && remaining > 0)
	{
		// 目前发送缓冲区剩余的待发送数据的长度
		size_t oldLen = outputBuffer_.readableBytes();
		if (oldLen + remaining >= highWaterMark_
			&& oldLen < highWaterMark_
			&& highWaterMarkCallback_)
		{
			loop_->addCB(
				std::bind(highWaterMarkCallback_, shared_from_this(), oldLen+remaining)
			);
		}
		outputBuffer_.append((char*)data + nwrote, remaining);
		if (!channel_->isWriting())
		{
			channel_->enableWriting(); // 这里一定要注册 channel 的写事件，否则 poller 不会给channel通知 epollout
		}
	}
}


void TcpConnection::shutdownInLoop() {
	if (!channel_->isWriting()) // 说明outputBuffer中的数据已经全部发送完成
	{
		socket_->shutdownWrite(); // 关闭写端
	}
}

void TcpConnection::connectEstablished() {
	setState(kConnected);
	channel_->enableReading(); // 向 poller 注册 channel 的 epollin 事件

	// 新连接建立，执行回调
	connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
	if (state_ == kConnected) {
		setState(kDisconnected);
		channel_->disableAll(); // 把 channel 的所有感兴趣的事件，从 poller 中 del 掉
		connectionCallback_(shared_from_this());
	}
	channel_->remove(); // 把 channel 从 poller 中删除掉
}

void TcpConnection::handleRead() {
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if (n > 0) {
		// 已建立连接的用户，有可读事件发生了，调用用户传入的回调操作onMessage
		messageCallback_(shared_from_this(), &inputBuffer_);
	} else if (n == 0) {
		handleClose();
	} else {
		errno = savedErrno;
		LOG_ERROR("TcpConnection::handleRead");
		handleError();
	}
}

void TcpConnection::handleWrite() {
	if (channel_->isWriting()) {
		int savedErrno = 0;
		ssize_t n = outputBuffer_.writeFd(channel_->fd(), &savedErrno);
		if (n > 0) {
			outputBuffer_.retrieve(n);
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
				if (writeCompleteCallback_) {
					// 唤醒loop_对应的thread线程，执行回调
					loop_->addCB(
						std::bind(writeCompleteCallback_, shared_from_this())
					);
				}
				if (state_ == kDisconnecting) {
					shutdownInLoop();
				}
			}
		} else {
			LOG_ERROR("TcpConnection::handleWrite");
		}
	} else {
		LOG_ERROR("TcpConnection fd=%d is down, no more writing \n", channel_->fd());
	}
}

// poller => channel::closeCallback => TcpConnection::handleClose
void TcpConnection::handleClose() {
	LOG_INFO("TcpConnection::handleClose fd=%d state=%d \n", channel_->fd(), (int)state_);
	setState(kDisconnected);
	channel_->disableAll();

	TcpConnectionPtr connPtr(shared_from_this());
	connectionCallback_(connPtr); // 执行连接关闭的回调
	closeCallback_(connPtr); // 关闭连接的回调  执行的是TcpServer::removeConnection回调方法
}

void TcpConnection::handleError() {
	int optval;
	socklen_t optlen = sizeof optval;
	int err = 0;
	if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		err = errno;
	} else {
		err = optval;
	}
	LOG_ERROR("TcpConnection::handleError name:%d - SO_ERROR:%d \n", id_, err);
}

