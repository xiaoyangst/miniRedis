/**
  ******************************************************************************
  * @file           : Acceptor.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/20
  ******************************************************************************
  */

#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress &listenAddr)
	: loop_(loop), acceptSocket_(socket(AF_INET, SOCK_STREAM, 0)), acceptChannel_(loop, acceptSocket_.fd()) {
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(true);
	acceptSocket_.setNonBlocking(true);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(
		std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
	acceptChannel_.disableAll();
	acceptChannel_.remove();
}

void Acceptor::listen()
{
	isListen_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);	// 得到一个第一次和服务器建立连接的 fd
	LOG_INFO("connfd %d accept",connfd);
	if (connfd >= 0)
	{
		if (newConnectionCallback_)	// 这个回调是 TcpServer 来设置，不会被用户设置
		{
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			close(connfd);
		}
	}
}
