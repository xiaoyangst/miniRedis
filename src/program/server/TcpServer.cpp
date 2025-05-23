#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cerrno>
#include <csignal>

#include "MsgInterflow.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Epoller.h"
#include "TcpConnection.h"
#include "TcpServer.h"

constexpr std::string IP = "127.0.0.1";
constexpr uint16_t PORT = 7777;
constexpr int MAX_EVENTS = 1024;
constexpr int BUFFER_SIZE = 4096;

volatile bool running = true;

void handle_signal(int signum) {
	std::cout << "\nReceived signal " << signum << ", shutting down server...\n";
	running = false;
}

int main() {
	signal(SIGINT, handle_signal);  // Ctrl+C
	signal(SIGTERM, handle_signal); // kill 命令

	auto loop = new EventLoop();
	InetAddress inet_address(IP, PORT);

	TcpServer tcp_server(loop, inet_address);

	// 设置回调

	tcp_server.setMessageCallback([](const TcpConnectionPtr &conn,Buffer *buf){
	  std::string msg = buf->retrieveAllAsString();
	  std::cout << "[RECEIVED] From " << conn->id() << ": " << msg << std::endl;

	  std::string reply = "PONG+OK";
	  conn->send(reply);
	});

	tcp_server.setWriteCompleteCallback([](const TcpConnectionPtr &conn){
		std::cout << "Send to Client" << std::endl;
	});

	tcp_server.start();    // 开始监听客户端的连接

	std::cout << "Server started on port " << PORT << "\n";

	loop->loop();    // 开启事件循环，监听客户端连接和消息事件

	return 0;
}
