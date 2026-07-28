#include "Image.h"
#include "BaseObject.h"
#include "BootScene.h"
#include "../ImGUI/imgui.h"
#include <wincodec.h>
#include <vector>
#include "CameraManager.h"
#include "DirectX3DManager.h"

using namespace DirectX3DManager;

using namespace DirectX;

Image::Image(const std::string& path, const float leftX, const float leftY)
	: BaseObject("Image") {
	// DirectXでは、2個の三角形を組み合わせて描画するイメージ
	//  |------------|
	//  |[0,1,2] /   |
	//  |       /    |
	//  |     /      |
	//  |   / [3,4,5]|
	//  |------------|
	this->texture_ = new Texture(path, leftX, leftY);
}

void Image::Init() {
	texture_->Init();
}

void Image::Update() {
	Camera* camera = CameraManager::getCurentCamera();
	if (camera == nullptr) return;

	XMMATRIX world = transform_.GetWorldMatrix();
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(
		0.0f, 1280.0f,
		720.0f, 0.0f,
		0.0f, 100.0f
	);
	ConstantBuffer cb = {};
	cb.wvpMat = XMMatrixTranspose(world * view * projection);
	cb.diffUse = { 1.0f, 0.0f, 0.0f, 1.0f };
	cb.isTexture = true;
	cb.isGray = texture_->IsGray();
	GetContext()->UpdateSubresource(texture_->GetConstantBuffer(), 0, nullptr, &cb, 0, 0);
}

void Image::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = texture_->GetVertexBuffer();
	ID3D11Buffer* constantBuffer = texture_->GetConstantBuffer();
	ID3D11SamplerState* samplerState = texture_->GetSampleState();
	ID3D11ShaderResourceView* shaderResourceView = texture_->GetShaderReasourceView();

	ChangeDrawWireFrameMode(false);
	GetContext()->IASetInputLayout(ShaderManager::inputLayout_);
	GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GetContext()->VSSetShader(ShaderManager::vertexShader_, nullptr, 0);
	GetContext()->PSSetShader(ShaderManager::pixelShader_, nullptr, 0);
	GetContext()->PSSetShaderResources(0, 1, &shaderResourceView);
	GetContext()->PSSetSamplers(0, 1, &samplerState);
	GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
	GetContext()->PSSetConstantBuffers(0, 1, &constantBuffer);

	GetContext()->Draw(6, 0);

	GetContext()->RSSetState(nullptr);
}

void Image::Release() {
	texture_->Release();
}
