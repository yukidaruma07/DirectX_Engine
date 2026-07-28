#include "Box.h"
#include <DirectXMath.h>
#include "DirectX3DManager.h"
#include "CameraManager.h"
#include "../ImGUI/imgui.h"

using namespace DirectX3DManager;
using namespace DirectX;

Box::Box(const Color& color, float width, float height)
	: BaseObject("Box") {
	vertices_[0] = { {0.0f, 0.0f, 0.0f}, color, {0, 0} }; // 左上 
	vertices_[1] = { {0.0f, width, 0.0f }, color, { 0, 1 } }; // 左下
	vertices_[2] = { {width, 0.0f, 0.0f}, color, {1, 0} }; // 右上

	vertices_[3] = { {width, 0.0f, 0.0f}, color, {1, 0} }; // 右上
	vertices_[4] = { {width, height, 0.0f }, color, { 1, 1 } }; // 右下
	vertices_[5] = { {0.0f, height, 0.0f}, color, {0, 1} }; // 左下
}

void Box::Init() {
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(vertices_);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// 参考： https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	result = GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer_);

	D3D11_SUBRESOURCE_DATA verticeData = {};
	verticeData.pSysMem = vertices_;

	result = GetDevice()->CreateBuffer(&bufferDesc, &verticeData, &vertexBuffer_);
}

void Box::Update() {
	Camera* camera = CameraManager::getCurentCamera();
	if (camera == nullptr) return;

	XMMATRIX world = transform_.GetWorldMatrix();
	XMMATRIX view = camera->getMatrix();
	XMMATRIX projection = camera->GetProjection();

	ConstantBuffer cb = {};
	cb.wvpMat = XMMatrixTranspose(world * view * projection);
	GetContext()->UpdateSubresource(constantBuffer_, 0, nullptr, &cb, 0, 0);
}

void Box::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	ChangeDrawWireFrameMode(false);
	GetContext()->PSSetShaderResources(0, 1, &nullSRV);
	GetContext()->IASetInputLayout(ShaderManager::inputLayout_);
	GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GetContext()->VSSetShader(ShaderManager::vertexShader_, nullptr, 0);
	GetContext()->PSSetShader(ShaderManager::pixelDebugShader_, nullptr, 0);
	GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer_);

	GetContext()->Draw(6, 0);

	GetContext()->RSSetState(nullptr);
}

void Box::Release() {
}
