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

constexpr std::string IP = "127.0.0.1";
constexpr uint16_t PORT = 8888;
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

	auto loop = std::make_shared<EventLoop>();
	InetAddress inet_address(IP, PORT);
	Acceptor acceptor(loop, inet_address);
	acceptor.listen();

	Epoller epoller(loop);

	loop->loop();

	std::cout << "Server started on port " << PORT << "\n";

	return 0;
}
