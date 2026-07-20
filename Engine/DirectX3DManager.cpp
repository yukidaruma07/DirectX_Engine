#include "DirectX3DManager.h"
#include "../GameEngine.hpp"
#include <Windows.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace DirectX3DManager {
	inline ID3D11Device* d3d11Device_ = nullptr;
	inline ID3D11DeviceContext* d3d11Context_ = nullptr;
	inline IDXGISwapChain* swapChain_ = nullptr;
	inline ID3D11RenderTargetView* renderTargetView_ = nullptr;
	inline ID3D11Texture2D* texture2D_ = nullptr;
	inline ID3D11Texture2D* depthStencilTexture_ = nullptr;
	inline ID3D11DepthStencilState* depthState_ = nullptr;
	inline ID3D11DepthStencilView* pDepthStencilView = nullptr;
	inline ID3D11RasterizerState* rasterizerState_ = nullptr;
	inline ID3D11RasterizerState* wireFrameRasterizerState_ = nullptr;
	inline ID3D11BlendState* blendState = nullptr;

	void DirectX3DManager::InitDirectX3D() {

		//参考：https://learn.microsoft.com/ja-jp/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
		// スワップチェインの設定
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = GameEngine::DEFAULT_WIDTH;		// 画面幅
		desc.BufferDesc.Height = GameEngine::DEFAULT_HEIGHT;	// 画面の高さ
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 
		desc.BufferDesc.RefreshRate.Numerator = 60;				// リフレッシュレートの分子 （※60/1FPSなら、60を指定する）
		desc.BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分母（※60/1FPSなら、1を指定する）
		desc.Windowed = TRUE;									// ウインドウモードであるかどうか
		desc.OutputWindow = GameEngine::GetWindowHandle();		// 出力するウインドウ
		desc.BufferCount = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ピクセルあたりのサンプル数
		desc.SampleDesc.Quality = 0;							// 品質レベル

		//参考：https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
		D3D_FEATURE_LEVEL level = {};
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&desc,
			&swapChain_,
			&d3d11Device_,
			&level,
			&d3d11Context_
		);

		swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture2D_);
		d3d11Device_->CreateRenderTargetView(texture2D_, NULL, &renderTargetView_);

		D3D11_TEXTURE2D_DESC depthTexDesc = {};
		depthTexDesc.Width = GameEngine::DEFAULT_WIDTH;
		depthTexDesc.Height = GameEngine::DEFAULT_HEIGHT;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;

		GetDevice()->CreateTexture2D(&depthTexDesc, nullptr, &depthStencilTexture_);

		//深度ステンシルビューの作成
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		GetDevice()->CreateDepthStencilView(depthStencilTexture_, &depthStencilViewDesc, &pDepthStencilView);

		//参考： https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
		// ビューポート：描画する範囲的なもの
		D3D11_VIEWPORT viewport = {};
		viewport.Width = GameEngine::DEFAULT_WIDTH;		// 描画範囲の幅
		viewport.Height = GameEngine::DEFAULT_HEIGHT;	// 描画範囲の高さ
		viewport.MinDepth = 0.0f;						// 描画範囲のZ軸の最小深さ
		viewport.MaxDepth = 1.0f;						// 描画範囲のZ軸の最大深さ
		viewport.TopLeftX = 0;							// 描画範囲の左上のX座標
		viewport.TopLeftY = 0;							// 描画範囲の左上のY座標
		d3d11Context_->RSSetViewports(1, &viewport);	// ビューポートを設定する

		//参考： https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; // レンダリング時に使用する塗りつぶしモード
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		GetDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizerState_);

		D3D11_RASTERIZER_DESC wireframeRasterizerDesc = {};
		wireframeRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; // レンダリング時に使用する塗りつぶしモード
		wireframeRasterizerDesc.CullMode = D3D11_CULL_NONE;
		wireframeRasterizerDesc.FrontCounterClockwise = FALSE;
		GetDevice()->CreateRasterizerState(&wireframeRasterizerDesc, &wireFrameRasterizerState_);

		d3d11Context_->RSSetState(rasterizerState_);

		//参考：https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_render_target_blend_desc
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE; //ブレンドを有効にする
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		GetDevice()->CreateBlendState(&blendDesc, &blendState); //ブレンドステートを作成する

		float blendFactor[4] = { 0, 0, 0, 0 };

		GetContext()->OMSetBlendState(
			blendState,
			blendFactor,
			0xFFFFFFFF
		);
	}

	void DirectX3DManager::Release() {
	}

	ID3D11Device* DirectX3DManager::GetDevice() { return d3d11Device_; }

	ID3D11DeviceContext* DirectX3DManager::GetContext() { return d3d11Context_; }

	IDXGISwapChain* DirectX3DManager::GetSwapChain() { return swapChain_;}

	ID3D11RenderTargetView* DirectX3DManager::GetRenderTargetView() { return renderTargetView_; }

	ID3D11Texture2D* DirectX3DManager::GetTexture2D() { return texture2D_; }

	ID3D11DepthStencilView* DirectX3DManager::GetDepthView()
	{
		return pDepthStencilView;
	}

	void ChangeDrawWireFrameMode(bool flag) {
		if (flag) {
			GetContext()->RSSetState(wireFrameRasterizerState_);
		}
		else {
			GetContext()->RSSetState(rasterizerState_);
		}
	}

}

namespace ShaderManager {

	void InitShader() {
		ID3DBlob* vsBlob = nullptr; 
		ID3DBlob* psBlobDebug = nullptr;
		ID3DBlob* psBlob = nullptr; 

		D3DCompileFromFile(
			L"Engine//TexturePixelShader.hlsl", //シェーダーのファイル
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", //シェーダーのメイン関数
			"ps_5_0", //シェーダーの種類
			D3DCOMPILE_ENABLE_STRICTNESS, //シェーダーコンパイルオプション
			0, //オプション
			&psBlob, //コンパイルしたデータを指定する
			NULL //コンパイル失敗時のデータを保存する
		);

		D3DCompileFromFile(
			L"Engine//DebugPixelShader.hlsl", //シェーダーのファイル
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", //シェーダーのメイン関数
			"ps_5_0", //シェーダーの種類
			D3DCOMPILE_ENABLE_STRICTNESS, //シェーダーコンパイルオプション
			0, //オプション
			&psBlobDebug, //コンパイルしたデータを指定する
			NULL //コンパイル失敗時のデータを保存する
		);

		D3DCompileFromFile(
			L"Engine//DebugVertexShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&vsBlob,
			NULL
		);

		DirectX3DManager::GetDevice()->CreateVertexShader(
			vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
			NULL,
			&vertexShader_);

		DirectX3DManager::GetDevice()->CreatePixelShader(
			psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
			NULL,
			&pixelShader_);

		/// 参考：https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, postion), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"TEXCOORD",       0, DXGI_FORMAT_R32G32_FLOAT,  0, offsetof(Vertex, uv),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DirectX3DManager::GetDevice()->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&inputLayout_
		);

	}
}