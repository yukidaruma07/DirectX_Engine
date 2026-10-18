#include "ServerSocketManager.h"
#include <vector>
#include <winsock.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <thread>
#include <csignal>

#pragma comment(lib, "Ws2_32.lib")

/// <summary>
/// サーバー側のソケットマネージャーです。
/// </summary>
namespace ServerSocketManager {

	inline std::vector<SocketPlayer> socketPlayerList;
	inline SOCKET serverSocket = {};
	inline const int PORT = 10000;

	void InitManager() {
		int result = 0;

		//WSADATAの参考：https://learn.microsoft.com/ja-jp/windows/win32/api/winsock2/ns-winsock2-wsadata
		WSADATA wsa = {};
		result = WSAStartup(MAKEWORD(2, 2), &wsa); //WinSockDLLの使用を開始する関数
		assert(result == 0 && "WSAのセットアップに失敗しました");

		///
		/// ソケットを作成
		/// 
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket == INVALID_SOCKET) {
			assert(serverSocket != INVALID_SOCKET);
			ReleaseApp();
			return;
		}

		SOCKADDR_IN addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = INADDR_ANY;

		///
		/// ソケットにIPを結びつける（=バインド）
		/// 
		result = bind(serverSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
		if (result == SOCKET_ERROR) {
			assert(result != SOCKET_ERROR && "バインドに失敗しました");
			ReleaseApp();
			return;
		}

		///
		/// 接続待ちにする（=リッスン）
		/// 
		result = listen(serverSocket, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			assert(result != SOCKET_ERROR && "listenに失敗しました");
			ReleaseApp();
			return;
		}

		u_long nonBlockFlag = 1; //ノンブロッキングのフラグ
		if (ioctlsocket(serverSocket, FIONBIO, &nonBlockFlag) == SOCKET_ERROR) {
			std::cerr << "エラー: " << WSAGetLastError() << '\n';
			ReleaseApp();
			return;
		}

		///
		/// while(true)によりブロックしちゃうため、別のスレッドで実行する
		/// 
		std::thread(RunMainLoop).detach();
	}

	void RunMainLoop() {
		///
		/// クライアントから受信する
		/// 
		while (true) {
			SOCKADDR_IN clientAddr = {};
			int size = sizeof(clientAddr);
			SOCKET clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &size);
			if (clientSocket == INVALID_SOCKET) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					std::cerr << "エラー: " << error << std::endl;
				}
			}
			else {
				u_long nonBlockFlag = 1; //ノンブロッキングのフラグ
				if (ioctlsocket(clientSocket, FIONBIO, &nonBlockFlag) == SOCKET_ERROR) {
					std::cerr << "エラー: " << WSAGetLastError() << std::endl;
					closesocket(clientSocket);
					continue;
				}

				SocketPlayer player = {};
				player.socket = clientSocket;
				player.id = socketPlayerList.size() + 1;
				player.connected = true;
				player.logined = false;
				socketPlayerList.push_back(player);
				std::cout << "誰かがログインしています" << std::endl;
			}

			///
			/// サーバー側のプレイヤーリスト
			/// 
			for (auto& player : socketPlayerList) {
				if (!player.connected) continue;	// プレイヤーが接続されていない場合は、飛ばす

				///
				/// データを受け取る（recv）。
				/// bufに受け取ったデータをポインタで入れる。
				/// recvは受け取れたバイト数を返す。
				/// 
				char buf[256] = {};
				int result = recv(player.socket, buf, sizeof(buf) - 1, 0);

				///
				/// 受け取ったバイト数が1以上。
				/// 何も受け取れなかった場合、-1（マクロ：SOCKET_ERROR）を返す。
				/// 
				if (result > 0) {
					buf[result] = '\0';
					player.rawBuffer.append(buf, result);

					size_t pos = 0;
					while ((pos = player.rawBuffer.find('\n')) != std::string::npos) {
						std::string line = player.rawBuffer.substr(0, pos);	// 受け取ったデータ
						player.rawBuffer.erase(0, pos + 1);

						if (line.empty()) continue;

						std::string type = ""; // 接続の種類

						if (type == "MeLogin" && !player.logined) {	// 新しいログインで、まだサーバーログインしていない
							player.name = "";	// 受け取ったデータから、名前を代入
							player.logined = true;

							///
							/// 新しくログインした人に、既にいる人のデータを返す処理
							/// 
							for (const auto& other : socketPlayerList) {
								if (&other == &player || !other.connected || !other.logined) continue;
								std::string type = "NewLogin";
								std::string name = other.name;
								std::string data = std::string("") + std::string("\n");	// 相手に送るデータ
								sendText(player.socket, data);
							}

							/// 
							/// 全員（※既にログインしている人）に、新しくログインした人のデータを送る
							/// 
							std::string data = "";	// NewLogin と nameを返す
							broadcast(socketPlayerList, data + "\n");
							std::cout << player.name << "さんがログインしました\n";
						}
						if (type == "MessageSend") {
							std::string message = "";
							std::cout << "プレイヤー（メッセージ送信）:" << player.name << ": " << message << "\n";
						}
						else if (type == "TransformMove") {
							broadcast(socketPlayerList, std::string(line) + "\n");
						}
						else {
							std::cout << "プレイヤー（その他）:" << player.name << ": " << line << "\n";
						}
					}
				}
				else if (result == SOCKET_ERROR) {
					int error = WSAGetLastError();
					if (error == WSAECONNRESET) {
						std::cout << "プレイヤー" << player.name << "が切断されました。";
						player.connected = false;
						broadcast(socketPlayerList, ("プレイヤー" + std::to_string(player.id) + "は切断されました。"));
					}
				}
			}
			Sleep(10);
		}
	}

	bool sendText(const SOCKET& socket, const std::string& text) {
		int result = SOCKET_ERROR;
		result = send(socket, text.c_str(), static_cast<int>(text.size()), 0);
		return result != SOCKET_ERROR;
	}

	void broadcast(const std::vector<SocketPlayer>& players, const std::string& text) {
		for (const auto& player : players) {
			if (player.connected) {
				sendText(player.socket, text);
			}
		}
	}

	void ReleaseApp() {
		for (auto& player : socketPlayerList) {
			if (player.socket != INVALID_SOCKET) {
				closesocket(player.socket);
			}
		}

		closesocket(serverSocket);
		WSACleanup();
	}
}