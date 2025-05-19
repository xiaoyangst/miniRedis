#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "MsgProtocol.h"
#include "MsgInterflow.h"

constexpr int PORT = 8888;
constexpr int CLIENT_COUNT = 3;
constexpr int MSG_PER_CLIENT = 100;

void client_task(int id) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "[Client " << id << "] Connection failed\n";
		return;
	}

	std::cout << "[Client " << id << "] Connected\n";

	for (int i = 0; i < MSG_PER_CLIENT; ++i) {
		std::string message = "Hello from client " + std::to_string(id) + " msg " + std::to_string(i);
		auto packed = MsgProtocol::pack(message);

		// 发送大量消息模拟粘包
		// 服务器不需要回复，客户端也不用接收，测试的时候需要关闭服务器回复消息的功能函数
		// 服务器来证明接受消息拆包没问题，客户端来证明封包发送消息没问题
		std::string glued = packed + packed + packed;

		if (!MsgInterflow::sendAll(sock, glued)) {
			std::cerr << "[Client " << id << "] Send failed\n";
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 稍微等待
	}

	close(sock);
	std::cout << "[Client " << id << "] Done\n";
}


int main() {
	std::vector<std::thread> clients;

	for (int i = 0; i < CLIENT_COUNT; ++i) {
		clients.emplace_back(client_task, i);
	}

	for (auto& th : clients) {
		th.join();
	}

	std::cout << "All clients finished.\n";
	return 0;
}
