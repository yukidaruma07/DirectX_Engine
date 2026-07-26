#pragma once
#include "BaseObject.h"
#include "Collider.h"
#include <DirectXMath.h>

class Collider;
class BoxCollider : public Collider {
private:
	BaseObject* parent_;	// コライダーの親
	DirectX::XMFLOAT3 boxSize_;
public:

	BoxCollider(BaseObject* parent, DirectX::XMFLOAT3 boxSize);
	~BoxCollider();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	DirectX::XMFLOAT3 GetBoxSize() const { return boxSize_; }
	bool IsBox(BoxCollider* boxCol);
};