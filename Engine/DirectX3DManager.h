#pragma once

#include <Windows.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>

/// <summary>
/// GPU（シェーダ）に送る構造体
/// </summary>
struct ConstantBuffer {
	DirectX::XMMATRIX wvpMat = DirectX::XMMatrixIdentity(); /// World行列、View行列、Projection行列を合わせた行列
	DirectX::XMFLOAT4 diffUse = {};							/// マテリアルの色
	DirectX::XMFLOAT4 ambient = {};							/// アンビエント
	DirectX::XMFLOAT4 speculer = {};						/// スペキュラー
	int isTexture;											/// テクスチャがあるか
	int isGray;												/// グレーにするか
	int isMosaic;
};

/// <summary>
/// 色の構造体（※0～1.0の範囲）
/// </summary>
struct Color {
	float r, g, b, a;

	static Color GetRed() { return {1.0, 0, 0, 1}; }
	static Color GetBlue() { return { 0, 1.0, 0, 1 }; }
	static Color GetGreen() { return { 0, 0, 1.0, 1 }; }
	static Color GetWhite() { return { 1.0, 1.0, 1.0, 1 }; }
	static Color GetBlack() { return { 0, 0, 0, 1 }; }
};

/// <summary>
/// 頂点の構造体
/// </summary>
struct Vertex {
	DirectX::XMFLOAT3 postion = {};		/// 頂点のローカル座標
	Color color = {};					/// 頂点の色
	DirectX::XMFLOAT2 uv = {};			/// テクスチャのUV座標（※Uを横、Vを縦とする）
	DirectX::XMFLOAT3 normal = {};	//法線
};

/// <summary>
/// Direct3Dを管理するための名前空間
/// </summary>
namespace DirectX3DManager {

	void InitDirectX3D();
	void Release();

	/// <summary>
	/// 描画デバイスを取得する関数
	/// </summary>
	/// <returns></returns>
	ID3D11Device* GetDevice();

	/// <summary>
	/// コンテキストを取得する関数
	/// </summary>
	/// <returns></returns>
	ID3D11DeviceContext* GetContext();

	/// <summary>
	/// スワップチェインを取得する関数
	/// </summary>
	/// <returns></returns>
	IDXGISwapChain* GetSwapChain();

	/// <summary>
	/// レンダーターゲットビューを取得する関数
	/// </summary>
	/// <returns></returns>
	ID3D11RenderTargetView* GetRenderTargetView();

	/// <summary>
	/// Texture2Dを取得する関数
	/// </summary>
	/// <returns></returns>
	ID3D11Texture2D* GetTexture2D();

	/// <summary>
	/// 深度バッファを取得する関数
	/// </summary>
	/// <returns></returns>
	ID3D11DepthStencilView* GetDepthView();

	/// <summary>
	/// 描画をワイドフレームモードにする関数
	/// </summary>
	void ChangeDrawWireFrameMode(bool flag);
}

/// <summary>
/// シェーダーを管理するための名前空間
/// </summary>
namespace ShaderManager {

	inline ID3D11VertexShader* vertexShader_ = nullptr;
	inline ID3D11PixelShader* pixelShader_ = nullptr;
	inline ID3D11PixelShader* pixelDebugShader_ = nullptr;
	inline ID3D11PixelShader* grayPixelShader_ = nullptr;
	inline ID3D11InputLayout* inputLayout_ = nullptr;

	void InitShader();


}