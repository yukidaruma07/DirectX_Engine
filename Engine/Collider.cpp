#include "Collider.h"
#include "CircleCollider.h"
#include "BoxCollider.h"

using namespace DirectX;

bool Collider::IsCirlce(const CircleCollider* aCol, const CircleCollider* bCol) {
	if (!aCol || !bCol) return false;
	DirectX::XMFLOAT3 aPos = aCol->GetPosition();
	DirectX::XMFLOAT3 bPos = bCol->GetPosition();
	DirectX::XMVECTOR vectorA = XMLoadFloat3(&aPos);
	DirectX::XMVECTOR vectorB = XMLoadFloat3(&bPos);

	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(vectorA, vectorB);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));
	const float merge = aCol->GetRadius() + bCol->GetRadius();
	return distance < merge;
}

bool Collider::IsBox(const BoxCollider* aCol, const BoxCollider* bCol) {
	if (!aCol || !bCol) return false;
	auto aPos = aCol->GetPosition();
	auto aSize = aCol->GetBoxSize();
	auto bPos = bCol->GetPosition();
	auto bSize = bCol->GetBoxSize();

	bool x = aPos.x - aSize.x / 2 < bPos.x + bSize.x / 2 &&
		aPos.x + aSize.x / 2 > bPos.x - bSize.x / 2;
	bool y = aPos.y - aSize.y / 2 < bPos.y + bSize.y / 2 &&
		aPos.y + aSize.y / 2 > bPos.y - bSize.y / 2;
	bool z = aPos.z - aSize.z / 2 < bPos.z + bSize.z / 2 &&
		aPos.z + aSize.z / 2 > bPos.z - bSize.z / 2;

	return x && y && z;
}
