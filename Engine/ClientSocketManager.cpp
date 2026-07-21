#include "ClientSocketManager.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cassert>
#include <string>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

namespace ClientSocketManager {
	inline SOCKET clientSocket = {};
}

void ClientSocketManager::InitManager() {
	int result = 0;
	WSADATA wsa = {};
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	assert(result == 0 && "WSAの初期化に失敗しました。");

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		assert(clientSocket != INVALID_SOCKET);
		ReleaseApp();
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	result = connect(clientSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
	if (result == SOCKET_ERROR) {
		assert(result == 0);
		ReleaseApp();
	}

	std::cout << "サーバーに接続しました\n";
	while (true) {
		char buf[256] = {};
		result = recv(clientSocket, buf, sizeof(buf) - 1, 0);
		if (result == 0) {
			std::cout << "サーバーから切断されました。";
			break;
		}
		if (result == SOCKET_ERROR) {
			std::cout << "recvの取得に失敗しました。";
			break;
		}

		buf[result] = '\0';
		std::string message(buf);
		std::cout << "[Server]" << message << "\n";

		std::cin >> message;
		send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
	}

	ReleaseApp();
}

void ClientSocketManager::ReleaseApp() {
	closesocket(clientSocket);
	WSACleanup();
}
