#pragma once
#include "BaseObject.h"

#include <DirectXMath.h>

class CircleCollider;
class BoxCollider;

enum class ColliderType {
	CirlceCollider,
	BoxCollider,
	UNKNOWN
};

class Collider : public BaseObject {
private:
protected:
	ColliderType type_;
protected:
	Collider(const std::string& name, const bool isDead = false)
		: BaseObject(name, isDead) {
		type_ = ColliderType::UNKNOWN;
	}
public:
	~Collider() {}

	ColliderType GetType() const { return type_; }

	bool IsCirlce(const CircleCollider* aCol, const CircleCollider* bCol);
	bool IsBox(const BoxCollider* aCol, const BoxCollider* bCol);
};