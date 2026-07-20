#include "BootScene.h"
#include "ObjectManager.h"
#include "Triangle.h"
#include "Image.h"
#include "LoggerManager.h"
#include "FBX.h"
#include "Box.h"

namespace {
	FBX* animeFbx = nullptr;
}

BootScene::BootScene()
	: BaseScene("BootScene") {
}

BootScene::~BootScene() {
}

void BootScene::Init() {
	DirectX::XMFLOAT3 vertexPos[3] = {
		{0.0f, 0.0f, 0.0f},
		{0.0f, 128.0f, 0.0f},
		{64.0f, 0.0f, 0.0f}
	};
	Color color[3] = {
		Color::GetRed(),
		Color::GetGreen(),
		Color::GetBlue()
	};
	ObjectManager::AddObject(new Triangle(color, vertexPos));
	ObjectManager::AddObject(new Image("test.PNG", 64, 64));
	//ObjectManager::AddObject(new FBX("Oden.fbx", {FBXPostionType::FBX_LEFTX_YUP_DEPTHX}));
	ObjectManager::AddObject(new Box(Color::GetRed(), 64, 64));

	animeFbx = new FBX("anime.fbx", { FBXPostionType::FBX_LEFTX_ZUP_DEPTHY });
	animeFbx->Init();
}

void BootScene::Update() {
	animeFbx->Update();
}

void BootScene::Draw() {
	FbxTime fbxTime = {};
	fbxTime.SetFrame(30);
	animeFbx->DrawAnime(fbxTime);
	//animeFbx->Draw();
	animeFbx->DrawObjectInfoImGUI();
}

void BootScene::Release() {
}
