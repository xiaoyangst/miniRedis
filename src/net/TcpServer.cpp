/**
  ******************************************************************************
  * @file           : TcpServer.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/21
  ******************************************************************************
  */

#include "TcpServer.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Utils.h"

TcpServer::TcpServer(std::shared_ptr<EventLoop> loop, const InetAddress &listenAddr)
	: loop_(loop), acceptor_(std::make_unique<Acceptor>(loop_, listenAddr))
	  , connectionCallback_(defaultConnectionCallback) {
	acceptor_->setNewConnectionCallback(
		[this](int sockfd, const InetAddress &peerAddr) {
		  this->newConnection(sockfd, peerAddr);
		}
	);
}

void TcpServer::start() {
	acceptor_->listen();
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
	InetAddress localAddr(Utils::getLocalAddr(sockfd));

	curConnId_++;

	TcpConnectionPtr conn(new TcpConnection(loop_,
											sockfd,
											localAddr,
											peerAddr));
	connections_[curConnId_] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		[this](const std::shared_ptr<TcpConnection> &conn) {
		  this->removeConnection(conn);
		}
	);

	loop_->addCB([conn]() {
	  conn->connectEstablished();
	});

}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
	connections_.erase(conn->id());
	loop_->addCB([conn]() {
	  conn->connectDestroyed();
	});
}


