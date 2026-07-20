#pragma once
#include "BaseObject.h"
#include <d3d11.h>
#include "DirectX3DManager.h"

/// <summary>
/// 三角形を描画するクラス。
/// </summary>
class Triangle : public BaseObject {
private:
    ID3D11Buffer* vertexBuffer_ = nullptr;      // 頂点バッファ
    Vertex vertices_[3] = {};                   // 頂点のデータ
    ID3D11Buffer* constantBuffer_ = nullptr;    // コンスタントバッファ
private:
    Color color_[3] = {};                       // 色のデータ
public:
    Triangle(const Color& color, DirectX::XMFLOAT3 postion[3]);
    Triangle(Color color[3], DirectX::XMFLOAT3 postion[3]);
    ~Triangle() {}

    void Init() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};