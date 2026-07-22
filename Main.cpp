#include <Windows.h>
#include "GameEngine.hpp"
#include "Engine//DirectX3DManager.h"
#include "Engine/SceneManager.h"
#include "Engine/ObjectManager.h"
#include "Engine/CameraManager.h"
#include "Engine/AudioManager.h"
#include "Engine/InputManager.h"
#include "Engine/DirectX2DManager.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

#define WINDOW_CLASS_NAME "GameEngine"
#define WINDOW_TITLE "MyGame"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Winmm.lib")

void initializeWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void initializeImGUI();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace DirectX3DManager;

namespace GameEngine {
	HWND hwnd = {};
	float deltaTime = 0.0f;

	HWND GetWindowHandle() {
		return hwnd;
	}

	float GetDeltaTime() {
		return deltaTime;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	initializeWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	InputManager::initialize(hInstance, GameEngine::GetWindowHandle());
	DirectX3DManager::InitDirectX3D();
	initializeImGUI();
	DirectX2DManager::Init();
	ShaderManager::InitShader();
	AudioManager::InitManager();
	SceneManager::InitManager();
	CameraManager::addCamera("RootCamera");
	CameraManager::setCurentCamera("RootCamera");

	Camera* currentCamera = CameraManager::getCurentCamera();
	currentCamera->setCameraPostion({ 0, 20, -5 });
	currentCamera->SetProjection(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f));

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			auto renderTargetView = GetRenderTargetView();
			GetContext()->OMSetRenderTargets(1, &renderTargetView, GetDepthView());
			GetContext()->ClearRenderTargetView(renderTargetView, GameEngine::BACKGROUND_COLOR);
			GetContext()->ClearDepthStencilView(GetDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			static DWORD beforeTime = timeGetTime();
			DWORD now = timeGetTime();
			DWORD subt = now - beforeTime;

			beforeTime = now;
			GameEngine::deltaTime = (float)subt / 1000;

			auto& io = ImGui::GetIO();
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			SceneManager::UpdateScene();
			SceneManager::DrawScene();
			ObjectManager::UpdateManager();

			#ifdef _DEBUG
				DirectX2DManager::DrawFontText(0, 0, L"あああ");

				auto currentCamera = CameraManager::getCurentCamera();
				auto cameraPos = currentCamera->getCameraPostion();
				auto targetPos = currentCamera->getFoucsPostion();
				ImGui::Begin("Main");
				ImGui::Text("DeltaTime: %2.2f", GameEngine::GetDeltaTime());
				ImGui::Text("Camera: %s", currentCamera == nullptr ? "" : currentCamera->getName().c_str());
				if (currentCamera != nullptr) {
					ImGui::SliderFloat("CameraPosX", &cameraPos.x, -1280.0f, 1280.0f);
					ImGui::SliderFloat("CameraPosY", &cameraPos.y, -1280.0f, 1280.0f);
					ImGui::SliderFloat("CameraPosZ", &cameraPos.z, -1280.0f, 1280.0f);
					ImGui::SliderFloat("CameraTargetX", &targetPos.x, -1280.0f, 1280.0f);
					ImGui::SliderFloat("CameraTargetY", &targetPos.y, -1280.0f, 1280.0f);
					ImGui::SliderFloat("CameraTargerZ", &targetPos.z, -1280.0f, 1280.0f);
					currentCamera->setCameraPostion(cameraPos);
					currentCamera->setFoucsPostion(targetPos);
				}
				ImGui::End();
			#endif

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			GetSwapChain()->Present(1, 0);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	switch (message) {
	case WM_CREATE: {
		break;
	}
	case WM_COMMAND: {
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void initializeWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	//WNDCLASSEXWの参考：https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/ns-winuser-wndclassexw
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX); //構造体のサイズ
	wndClass.hInstance = hInstance; //インスタンス
	wndClass.lpszClassName = WINDOW_CLASS_NAME; //クラスの名前
	wndClass.lpfnWndProc = WndProc; //ウインドウプロシージャ
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION); //アイコン（参考：https://learn.microsoft.com/ja-jp/windows/win32/menurc/about-icons）
	wndClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO); //小さいアイコン
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW); //カーソル（参考：https://learn.microsoft.com/ja-jp/windows/win32/menurc/about-cursors）
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景
	RegisterClassEx(&wndClass); //ウインドウクラスを登録する関数

	int width = GetPrivateProfileInt("SCREEN", "Width", GameEngine::DEFAULT_WIDTH, "./config.ini");
	int height = GetPrivateProfileInt("SCREEN", "Height", GameEngine::DEFAULT_HEIGHT, "./config.ini");
	char title[64] = {};
	GetPrivateProfileString("SCREEN", "Title", WINDOW_TITLE, title, 64, "./config.ini");

	//CreateWindowの参考：https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-createwindoww
	GameEngine::hwnd = CreateWindow(
		WINDOW_CLASS_NAME, //クラスの名前（※WNDCLASSEXと同じクラス名を指定する）
		title, //ウインドウの名前
		WS_OVERLAPPEDWINDOW, //ウインドウスタイル（参考：https://learn.microsoft.com/ja-jp/windows/win32/winmsg/window-styles）
		CW_USEDEFAULT, CW_USEDEFAULT, //ウインドウのX座標＆Y座標
		width, height, //ウインドウの幅＆高さ
		NULL, //親ウインドウ（※親ウインドウがいない場合はNULLで問題ない）
		NULL, //メニューのハンドル（※メニューがない場合はNULLで問題ない）
		hInstance, //インスタンス
		NULL //パラメータ
	);

	ShowWindow(GameEngine::GetWindowHandle(), nCmdShow);
	UpdateWindow(GameEngine::GetWindowHandle());
}

void initializeImGUI() {
	HWND hwnd = GameEngine::GetWindowHandle();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ID3D11Device* device = (ID3D11Device*)GetDevice();
	ID3D11DeviceContext* deviceContext = (ID3D11DeviceContext*)GetContext();
	ImGui_ImplDX11_Init(device, deviceContext);
}
