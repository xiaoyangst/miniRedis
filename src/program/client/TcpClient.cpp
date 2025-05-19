#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

constexpr int PORT = 6379;

int main() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Connection failed\n";
		return 1;
	}

	std::string command = "PING\r\n";
	send(sock, command.c_str(), command.size(), 0);

	char buffer[1024];
	int bytes = recv(sock, buffer, sizeof(buffer), 0);
	if (bytes > 0) {
		std::string response(buffer, bytes);
		std::cout << "Server reply: " << response << "\n";
	}

	close(sock);
}
