#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "MsgInterflow.h"
#include "Socket.h"

constexpr std::string IP = "127.0.0.1";
constexpr uint16_t PORT = 8888;

int main() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cerr << "Failed to create socket\n";
		return 1;
	}

	InetAddress serverAddr(IP, PORT);  // 127.0.0.1, 8888
	Socket clientSock(sock);
	clientSock.setReuseAddr(true);
	// clientSock.setNonBlocking(true);	// 不要设置非阻塞，否则 connect 会直接返回，导致连接不上服务器

	clientSock.connect(serverAddr);

	std::cout << "Connected to server on port " << PORT << ". Type 'exit' to quit.\n";

	while (true) {
		std::cout << "> ";
		std::string command;
		std::getline(std::cin, command);

		if (command.empty()) continue;

		if (command == "exit" || command == "quit") {
			break;
		}

		command += "\r\n";

		if (!MsgInterflow::sendMsg(sock, command)) {
			std::cerr << "Send failed\n";
			break;
		}

		std::string response;
		if (!MsgInterflow::recvMsg(sock, response)) {
			std::cerr << "Recv failed\n";
			break;
		}

		std::cout << "Server: " << response;
	}

	close(sock);
	std::cout << "Client exited.\n";
	return 0;
}
