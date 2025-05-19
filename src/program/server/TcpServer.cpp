#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

constexpr int PORT = 6379;
constexpr int MAX_EVENTS = 1024;
constexpr int BUFFER_SIZE = 4096;

int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	bind(server_fd, (sockaddr *)&addr, sizeof(addr));
	listen(server_fd, SOMAXCONN);
	set_nonblocking(server_fd);

	int epoll_fd = epoll_create1(0);
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

	std::cout << "Server started on port " << PORT << "\n";

	epoll_event events[MAX_EVENTS];

	while (true) {
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		for (int i = 0; i < nfds; ++i) {
			int fd = events[i].data.fd;

			if (fd == server_fd) {
				int client_fd = accept(fd, nullptr, nullptr);
				set_nonblocking(client_fd);
				epoll_event client_ev;
				client_ev.events = EPOLLIN | EPOLLET;
				client_ev.data.fd = client_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
				std::cout << "New client connected: FD=" << client_fd << "\n";
			} else {
				char buffer[BUFFER_SIZE] = {0};
				int bytes = recv(fd, buffer, BUFFER_SIZE, 0);
				if (bytes <= 0) {
					std::cout << "Client disconnected: FD=" << fd << "\n";
					close(fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
				} else {
					std::string message(buffer, bytes);
					std::cout << "Received: " << message << " from FD=" << fd << "\n";
					std::string reply = "+PONG\r\n";  // 模拟 RESP 回复
					send(fd, reply.c_str(), reply.size(), 0);
				}
			}
		}
	}
	close(server_fd);
}
