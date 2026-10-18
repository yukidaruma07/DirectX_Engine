#pragma once
#include <string>
#include <vector>
#include "Transform.h"


/// <summary>
/// クライアントが認識しているプレイヤーリスト
/// </summary>
inline std::vector<std::string> playerList = std::vector<std::string>();

const std::string PLAYER_NAME = "Player01";

namespace ClientSocketManager {
	inline SOCKET serverSocket = {};

	void Init();
	void RunMainLoop();
	void Release();

	void sendMessage(const std::string& message);
	void sendRawData(const std::string& message);
	void sendTransform(const Transform& transform);
}