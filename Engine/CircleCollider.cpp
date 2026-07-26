#include "CircleCollider.h"
#include "FBX.h"

using namespace DirectX;

namespace {
	FBX* fbx_ = nullptr;
}

CircleCollider::CircleCollider(BaseObject* parent, int radius)
	: Collider("CircleCollider", true) {
	this->parent_ = parent;
	this->radius_ = radius;
	isShowImGUI_ = false; //直接描画するわけではないため、ImGUIの表示はfalse
	this->type_ = ColliderType::CirlceCollider;
}

CircleCollider::~CircleCollider()
{
}

void CircleCollider::Init()
{
	fbx_ = new FBX("Engine/CircleCollider.fbx", { FBXPostionType::LEFTX_ZUP_DEPTHY });
	fbx_->Init();
}

void CircleCollider::Update()
{
	postion_ = parent_->GetPosition();
	fbx_->Update();
	fbx_->SetPosition(postion_);
}

void CircleCollider::Draw()
{
	fbx_->EnableWireFrame();
	fbx_->Draw();

}

void CircleCollider::Release()
{
}

bool CircleCollider::IsCircle(CircleCollider* circleCol) {
	XMFLOAT3 aPos = GetPosition();
	XMFLOAT3 bPos = circleCol->GetPosition();
	XMVECTOR vectorA = XMLoadFloat3(&aPos);
	XMVECTOR vectorB = XMLoadFloat3(&bPos);

	XMVECTOR diff = XMVectorSubtract(vectorA, vectorB);
	float distance = XMVectorGetX(XMVector3Length(diff));
	const float merge = radius_ + circleCol->GetRadius();
	if (distance < merge) {
		return true;
	}
	return false;
}
