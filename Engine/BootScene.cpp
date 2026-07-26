#include "BootScene.h"
#include "ObjectManager.h"
#include "Triangle.h"
#include "Image.h"
#include "LoggerManager.h"
#include "FBX.h"
#include "Box.h"
#include "AudioManager.h"
#include "BoxCollider.h"

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
	auto obj = ObjectManager::AddObject(new FBX("Oden.fbx", {FBXPostionType::LEFTX_YUP_DEPTHZ}));
	auto col = new BoxCollider(obj, { 2.0f, 5.0f, 1.0f });
	col->Init();
	obj->GetColiderList().push_back(col);
	ObjectManager::AddObject(new Box(Color::GetRed(), 64, 64));

	//int id = AudioManager::LoadMP3("Bossa_Latte.mp3");
	//int id = AudioManager::Load("Bossa_Latte.mp3");
	//AudioManager::PlayMP3(id);
	//AudioManager::Play(id);

	//animeFbx = new FBX("anime.fbx", { FBXPostionType::LEFTX_ZUP_DEPTHY });
	//animeFbx->Init();
	//animeFbx->GetColiderList().push_back(new BoxCollider(animeFbx, {1.0f, 1.0f, 1.0f}));
}

void BootScene::Update() {
	//animeFbx->Update();
}

void BootScene::Draw() {
	//FbxTime fbxTime = {};
	//fbxTime.SetFrame(30);
	//animeFbx->DrawAnime();
	//animeFbx->Draw();
	//animeFbx->DrawObjectInfoImGUI();
}

void BootScene::Release() {
}
