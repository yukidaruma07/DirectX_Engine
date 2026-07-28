#pragma once
#include "BaseData.h"
#include <DirectXMath.h>

class Transform : public BaseData {
private:
public:
	DirectX::XMFLOAT3 postion_;                 // オブジェクトの場所
	DirectX::XMFLOAT3 velocity_;                // オブジェクトのベクトル
	DirectX::XMFLOAT3 rotation_;                // オブジェクトの回転
	DirectX::XMFLOAT3 scale_;                   // オブジェクトの大きさ

	Transform() {
		postion_ = { 0,0,0 };
		velocity_ = { 0,0,0 };
		rotation_ = { 0,0,0 };
		scale_ = { 1,1,1 };
	};
	~Transform() {};

	DirectX::XMMATRIX GetWorldMatrix() {
		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationZ(rotation_.z) * DirectX::XMMatrixRotationX(rotation_.x) * DirectX::XMMatrixRotationY(rotation_.y);
		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(postion_.x, postion_.y, postion_.z);
		return scaleMat * rotMat * transMat;
	}

};