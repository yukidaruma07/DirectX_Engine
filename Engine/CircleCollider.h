#pragma once
#include "BaseObject.h"
#include "Collider.h"

/// <summary>
/// 当たり判定で必要となる、円のコライダー
/// </summary>
class CircleCollider : public BaseObject, public Collider {
private:
	BaseObject* parent_;	// コライダーの親
	int radius_;			// コライダーの半径
public:
	CircleCollider(BaseObject* parent, int radius);
	~CircleCollider();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	/// <summary>
	/// 円のコライダーの半径を返す関数。
	/// </summary>
	int GetRadius() const { return radius_; }

	/// <summary>
	/// 円と円が当たっているかを返す関数。
	/// </summary>
	/// <param name="circleCol">判定したい円のコライダー</param>
	/// <returns>当たっている場合はtrue、当たっていない場合はfalseを返す。</returns>
	bool IsCircle(CircleCollider* circleCol);
};