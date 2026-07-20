#pragma once
#include "DirectX3DManager.h"
#include <string>
#include <wincodec.h>
#include <vector>
#include "BaseObject.h"
#include "Texture.h"

/// <summary>
/// 画像を表示するクラス
/// </summary>
class Image : public BaseObject {
private:
	Texture* texture_;	// 画像を表示するためのテクスチャデータ
public:
	Image(const std::string& path, const float leftX, const float leftY);
	~Image() {};
	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};
