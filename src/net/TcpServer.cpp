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
#include "Logger.h"
#include "Utils.h"
#include <iostream>

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
	: loop_(loop), acceptor_(new Acceptor(loop_, listenAddr))
	  , messageCallback_(defaultMessageCallback)
	  , connectionCallback_(defaultConnectionCallback), curConnId_(1) {
	acceptor_->setNewConnectionCallback(
		[this](int sockfd, const InetAddress &peerAddr) {
		  this->newConnection(sockfd, peerAddr);
		}
	);
}

void TcpServer::start() {
	LOG_INFO("Server start");
	acceptor_->listen();
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {

	LOG_INFO("Have newConnection");

	InetAddress localAddr(Utils::getLocalAddr(sockfd));

	curConnId_++;

	TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_,
															curConnId_,
															sockfd,
															localAddr,
															peerAddr);
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

void TcpServer::defaultConnectionCallback(const TcpConnectionPtr &conn) {
	if (conn->connected()) {
		std::cout << "[INFO] New connection from " << conn->peerAddress().toIpPort()
				  << " (name: " << conn->id() << ")\n";
	} else {
		std::cout << "[INFO] Connection " << conn->id() << " is closed.\n";
	}
}

void TcpServer::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer) {
	std::string msg = buffer->retrieveAllAsString();
	std::cout << "[RECEIVED] From " << conn->id() << ": " << msg << std::endl;
}

TcpServer::~TcpServer() {
	for (auto &item : connections_) {
		TcpConnectionPtr conn(item.second);
		item.second.reset();

		// 销毁连接
		conn->getLoop()->addCB(
			std::bind(&TcpConnection::connectDestroyed, conn)
		);
	}
	loop_->quit();
}
