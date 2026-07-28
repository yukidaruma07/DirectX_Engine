#include "BoxCollider.h"
#include "FBX.h"

namespace {
	FBX* fbx_ = nullptr;
}

BoxCollider::BoxCollider(BaseObject* parent, DirectX::XMFLOAT3 boxSize)
	: Collider("BoxCollider", true) {
	this->parent_ = parent;
	this->boxSize_ = boxSize;
	isShowImGUI_ = false; //直接描画するわけではないため、ImGUIの表示はfalse
	this->type_ = ColliderType::BoxCollider;
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::Init()
{
	fbx_ = new FBX("Engine/BoxCollider.fbx", {FBXPostionType::LEFTX_ZUP_DEPTHY});
	fbx_->Init();
}

void BoxCollider::Update()
{
	float x = parent_->GetPosition().x + boxSize_.x / 2;
	float y = parent_->GetPosition().y + boxSize_.y / 2;
	float z = parent_->GetPosition().z + boxSize_.z / 2;
	transform_.postion_ = DirectX::XMFLOAT3{x, y, z};
	fbx_->Update();
	fbx_->transform_.postion_ = parent_->transform_.postion_;
	fbx_->transform_.scale_ = boxSize_;
}

void BoxCollider::Draw()
{
	fbx_->EnableWireFrame();
	fbx_->Draw();
}

void BoxCollider::Release()
{
}

bool BoxCollider::IsBox(BoxCollider* boxCol) {
	bool x = GetPosition().x - boxSize_.x / 2 < boxCol->GetPosition().x + boxCol->GetBoxSize().x / 2 &&
		GetPosition().x + boxSize_.x / 2 > boxCol->GetPosition().x - boxCol->GetBoxSize().x / 2;
	bool y = GetPosition().y - boxSize_.y / 2 < boxCol->GetPosition().y + boxCol->GetBoxSize().y / 2 &&
		GetPosition().y + boxSize_.y / 2 > boxCol->GetPosition().y - boxCol->GetBoxSize().y / 2;
	bool z = GetPosition().z - boxSize_.z / 2 < boxCol->GetPosition().z + boxCol->GetBoxSize().z / 2 &&
		GetPosition().z + boxSize_.z / 2 > boxCol->GetPosition().z - boxCol->GetBoxSize().z / 2;

	return x && y && z;
}
