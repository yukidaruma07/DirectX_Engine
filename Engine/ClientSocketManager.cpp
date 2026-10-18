#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cassert>
#include <string>
#include <iostream>
#include "ClientSocketManager.h"
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

void ClientSocketManager::Init() {
	int result = 0;
	WSADATA wsa = {};
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	assert(result == 0 && "WSAの初期化に失敗しました。");

	///
	/// ソケットを作成
	/// 
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		assert(serverSocket != INVALID_SOCKET);
		Release();
		return;
	}

	///
	/// 接続する
	/// 
	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	result = connect(serverSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
	if (result == SOCKET_ERROR) {
		assert(result == 0);
		Release();
		return;
	}

	std::string type = "MeLogin"; // 自分がログインしたことを示すパケット名
	std::string name = ""; // 自分の名前
	std::string sendData = "";
	send(serverSocket, sendData.data(), sendData.size(), 0);

	///
	/// while(true)によりブロックしちゃうため、別のスレッドで実行する
	/// 
	std::thread(RunMainLoop).detach();
}

void ClientSocketManager::RunMainLoop() {
	std::string rawBuffer = "";
	while (true) {

		///
		/// データを受け取る（recv）。
		/// bufに受け取ったデータをポインタで入れる。
		/// recvは受け取れたバイト数を返す。
		/// 
		char buf[256] = {};
		int result = recv(serverSocket, buf, sizeof(buf) - 1, 0);
		if (result < 0) {
			std::cout << "サーバーから切断されました。";
			break;
		}


		rawBuffer.append(buf, result);

		size_t pos = 0;
		while ((pos = rawBuffer.find('\n')) != std::string::npos) {
			std::string rawData = rawBuffer.substr(0, pos);
			rawBuffer.erase(0, pos + 1);
			if (rawData.empty()) continue;

			std::string type = ""; // 送られるデータの種類
			std::string name = ""; // プレイヤーの名前

			///
			/// type（送られるデータ）の種類
			/// NewLogin：新しくログインした人がいる
			/// TransformMove：プレイヤーが動いた場合
			/// 
			if (type == "NewLogin") {
				if (name == PLAYER_NAME) continue;	// 自分と同じ名前の場合は、飛ばす

				///
				/// クライアント側に、新しくログインした人を追加する
				/// 
				bool exist = false;
				for (std::string player : playerList) {	// 
					if (player == name) {
						exist = true;	// 存在している場合はtrueにする
					}
				}

				if (!exist) {
					// 
					// 最後まで見て存在しない場合は、追加する
					//
					playerList.push_back(name);
				}

			}
			else if (type == "TransformMove") {
				for (std::string player : playerList) {
					///
					/// プレイヤーが動いた時の処理を実装する
					/// 
				}
			}

		}
	}
}

void ClientSocketManager::Release() {
	closesocket(serverSocket);
	WSACleanup();
}

void ClientSocketManager::sendMessage(const std::string& message) {
	std::string type = "SendMessage"; // 送られるデータの種類
	std::string name = PLAYER_NAME; // 自分の名前
	std::string rawData = ""; // 送るデータ
	send(serverSocket, rawData.c_str(), static_cast<int>(rawData.size()), 0);
}

void ClientSocketManager::sendRawData(const std::string& message) {
	send(serverSocket, message.c_str(), static_cast<int>(message.size()), 0);
}

void ClientSocketManager::sendTransform(const Transform& transform) {
	std::string type = "TransformMove"; // 送られるデータの種類
	std::string name = PLAYER_NAME; // 自分の名前
	std::string rawData = ""; // 送るデータ
	send(serverSocket, rawData.c_str(), static_cast<int>(rawData.size()), 0);
}
