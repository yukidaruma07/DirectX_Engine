#pragma once
#include "BaseObject.h"
#include <d3d11.h>
#include "DirectX3DManager.h"

/// <summary>
/// 四角形を描画するためのクラス
/// </summary>
class Box : public BaseObject {
private:
    ID3D11Buffer* vertexBuffer_ = nullptr;      // 頂点バッファ
    Vertex vertices_[6] = {};                   // 頂点のデータ
    ID3D11Buffer* constantBuffer_ = nullptr;    // コンスタントバッファ
private:
    float width = 0.0f, height = 0.0f;
    Color color_ = {};
public:
    Box(const Color& color, float width, float height);
    ~Box() {}

    void Init() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};