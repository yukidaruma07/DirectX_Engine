#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <winsock.h>
#include <vector>

struct SocketPlayer {
	SOCKET socket = INVALID_SOCKET;
	int id = -1;
	bool connected = false;
	bool logined = false;
	std::string name = "";
	std::string rawBuffer = "";
};

/// <summary>
/// ソケット通信で、サーバー側のソケット通信を管理する名前空間です。
/// </summary>
namespace ServerSocketManager {

	void InitManager();
	void RunMainLoop();

	bool sendText(const SOCKET& socket, const std::string& text);
	void broadcast(const std::vector<SocketPlayer>& players, const std::string& text);

	void ReleaseApp();

}