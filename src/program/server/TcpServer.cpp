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

constexpr int PORT = 8888;
constexpr int MAX_EVENTS = 1024;
constexpr int BUFFER_SIZE = 4096;

volatile bool running = true;

void handle_signal(int signum) {
	std::cout << "\nReceived signal " << signum << ", shutting down server...\n";
	running = false;
}

int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int set_reuseaddr(int fd) {
	int opt = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

int main() {
	signal(SIGINT, handle_signal);  // Ctrl+C
	signal(SIGTERM, handle_signal); // kill 命令

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return 1;
	}

	set_reuseaddr(server_fd);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(server_fd, SOMAXCONN) < 0) {
		perror("listen");
		return 1;
	}

	set_nonblocking(server_fd);

	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("epoll_create1");
		return 1;
	}

	epoll_event ev{};
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

	std::cout << "Server started on port " << PORT << "\n";

	epoll_event events[MAX_EVENTS];

	while (true) {
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0) {
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; ++i) {
			int fd = events[i].data.fd;

			if (fd == server_fd) {
				// 新客户端连接
				while (true) {
					int client_fd = accept(server_fd, nullptr, nullptr);
					if (client_fd < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						perror("accept");
						continue;
					}

					set_nonblocking(client_fd);
					epoll_event client_ev{};
					client_ev.events = EPOLLIN | EPOLLET;
					client_ev.data.fd = client_fd;
					epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

					std::cout << "New client connected: FD=" << client_fd << "\n";
				}
			} else {

				std::string recv_msg;
				if (!MsgInterflow::recvMsg(fd,recv_msg)){
					std::cerr<<"RecvMsg failed\n";
				}
				std::cout << "Received: " << recv_msg << " from FD=" << fd << "\n";

				/*
				std::string reply = "+PONG\r\n";
				if (!MsgInterflow::sendMsg(fd,reply)){
					std::cerr<<"SendMsg failed\n";
				}
				*/

				/*
					// 处理客户端数据
					char buffer[BUFFER_SIZE];
					while (true) {
						int bytes = recv(fd, buffer, sizeof(buffer), 0);
						if (bytes > 0) {
							std::string message(buffer, bytes);
							// unpack_msg
							auto unpack_msg = MsgProtocol::unpack(message);
							std::cout << "Received: " << unpack_msg << " from FD=" << fd << "\n";

							std::string reply = "+PONG\r\n";
							// pack_msg
							auto pack_msg = MsgProtocol::pack(reply);
							send(fd, pack_msg.c_str(), pack_msg.size(), 0);
						} else if (bytes == 0) {
							std::cout << "Client disconnected: FD=" << fd << "\n";
							close(fd);
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
							break;
						} else {
							if (errno == EAGAIN || errno == EWOULDBLOCK) {
								break; // 所有数据读取完毕
							} else {
								perror("recv");
								close(fd);
								epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
								break;
							}
						}
					}

				 */
			}
		}
	}

	close(server_fd);
	return 0;
}
