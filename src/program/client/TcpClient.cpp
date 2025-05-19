#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "MsgInterflow.h"

constexpr int PORT = 8888;

int main() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cerr << "Failed to create socket\n";
		return 1;
	}

	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Connection failed\n";
		return 1;
	}

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
			std::cerr<< "Send failed\n";
			break;
		}

		/*
		// pack msg
		auto pack_msg = MsgProtocol::pack(command);

		if (send(sock, pack_msg.c_str(), pack_msg.size(), 0) < 0) {
			std::cerr << "Send failed\n";
			break;
		}
		 */

		std::string response;
		if (!MsgInterflow::recvMsg(sock,response)){
			std::cerr<<"Recv failed\n";
			break;
		}

		std::cout << "Server: " << response;

		/*
		char buffer[1024] = {0};
		int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
		if (bytes <= 0) {
			std::cerr << "Disconnected by server\n";
			break;
		}

		std::string response(buffer, bytes);

		// unpack msg
		auto unpack_msg = MsgProtocol::unpack(response);
		std::cout << "Server: " << unpack_msg;
		 */
	}

	close(sock);
	std::cout << "Client exited.\n";
	return 0;
}
