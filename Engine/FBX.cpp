#include "FBX.h"
#include "fbxsdk.h"
#include "DirectX3DManager.h"
#include "BootScene.h"
#include "Texture.h"
#include <vector>
#include "../ImGUI/imgui.h"
#include "CameraManager.h"
#include "../GameEngine.hpp"
#include "LoggerManager.h"
#include <DirectXCollision.h>
#include <filesystem>
#include "CSVManager.h"
#include <fstream>

using namespace DirectX3DManager;

using namespace fbxsdk;
using namespace DirectX;

FBX::FBX(const std::string fName, FBXLoadOption fbxOption)
	: BaseObject("FBX") {
	this->path_ = fName;
	fbxLoadOption_ = fbxOption;
	isShowTexture_ = true;
	indexCount_ = -1;
	materialCount_ = -1;
	polygonCount_ = -1;
	vertexCount_ = -1;
	materials_.clear();
	indexBuffer_.clear();
	pVertexBuffer_ = nullptr;
	vertices_.clear();

	nowFrame = 0, animSpeed = 1.0f;
	startFrame = 0, endFrame = 0;
}

FBX::~FBX() {
}

void FBX::Init() {
	if (!std::filesystem::exists(path_.c_str())) {
		MessageBox(NULL, "FBXファイルが存在しません。", NULL, MB_OK);
		return;
	}

	fbxManager_ = FbxManager::Create();
	fbxImporter_ = FbxImporter::Create(fbxManager_, "imp");
	fbxImporter_->Initialize(path_.c_str(), -1, fbxManager_->GetIOSettings());
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxscene");
	fbxImporter_->Import(fbxScene);

	FbxGeometryConverter converter(fbxManager_);
	converter.Triangulate(fbxScene, true);	// fbxを三角化する

	FbxNode* rootNode = fbxScene->GetRootNode();
	FbxNode* node = rootNode->GetChild(0); //結合済み前提
	FbxMesh* mesh = nullptr;
	for (int i = 0; i < rootNode->GetChildCount(); i++) {	// 根っこから子ノードをループする
		FbxNode* childNode = rootNode->GetChild(i);	// 子ノード
		if (childNode->GetMesh() != nullptr) {	// メッシュがnullじゃなくなるまで
			mesh = childNode->GetMesh();	// メッシュを代入
			node = childNode;	// メッシュがnullじゃないときの、ノードを代入する
			break;
		}
	}

	vertexCount_ = mesh->GetControlPointsCount();	//頂点数を取得する
	polygonCount_ = mesh->GetPolygonCount();		//ポリゴンを取得する
	materialCount_ = node->GetMaterialCount();		//マテリアルを取得する
	//indexMaterialCount_.resize(materialCount_);

	InitVertex(mesh);		//頂点バッファを初期化する
	InitIndex(mesh);		//インデックスバッファを初期化する
	InitConstantBuffer();	//コンスタントバッファ（GPUに送るデータ）を初期化する
	InitMaterial(node);		//マテリアルを初期化する
	InitSkeleton(mesh);

}

void FBX::InitVertex(FbxMesh* mesh) {

	FbxNode* node = mesh->GetNode();

	FbxLayer* layer = mesh->GetLayer(0);
	FbxLayerElementMaterial* matLayer = layer->GetMaterials();
	FbxLayerElement::EMappingMode mapping = matLayer->GetMappingMode();


	for (DWORD poly = 0; poly < polygonCount_; poly++) {	//ポリゴン分ループする
		for (int vertexCount = 0; vertexCount < 3; vertexCount++) {		//頂点分ループする（※結合済み前提
			Vertex vertex = {};

			int index = mesh->GetPolygonVertex(poly, vertexCount);
			FbxVector4 pos = mesh->GetControlPointAt(index);	//頂点座標（ローカル）

			LoggerManager::InfoDebug(fbxImporter_->GetFileName().Buffer());
			LoggerManager::InfoDebug(std::to_string(vertexCount) + "ローカル座標：" + std::to_string((float)pos[0]) + "," + std::to_string((float)pos[1]) + "," + std::to_string((float)pos[2]));

			if (fbxLoadOption_.postionType == FBX_LEFTX_YUP_DEPTHX) {
				vertex.postion.x = (float)pos[0];			//頂点のX座標を代入する
				vertex.postion.y = (float)pos[1];			//頂点のY座標を代入する
				vertex.postion.z = (float)pos[2];			//頂点のZ座標を代入する
			}
			else if (fbxLoadOption_.postionType == FBX_LEFTX_ZUP_DEPTHY) {
				vertex.postion.x = (float)pos[0];			//頂点のX座標を代入する
				vertex.postion.y = (float)pos[2];			//頂点のY座標を代入する
				vertex.postion.z = (float)pos[1];			//頂点のZ座標を代入する
			}


			FbxLayerElementUV* uvLayer = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertexCount);
			FbxVector2 uv = uvLayer->GetDirectArray().GetAt(uvIndex);	//UV座標を取得する
			vertex.uv.x = (float)uv.mData[0];					//UV座標の横軸を代入する
			vertex.uv.y = (float)(1.0f - uv.mData[1]);		//UV座標の縦軸を代入する

			vertices_.push_back(vertex);
		}
	}
	vertexCount_ = vertices_.size();

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * vertexCount_;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices_.data();			//頂点データ

	HRESULT hr = GetDevice()->CreateBuffer(&bd, &data, &pVertexBuffer_);	//頂点バッファを作成する
}

void FBX::InitIndex(FbxMesh* mesh) {
	indexBuffer_.resize(materialCount_);
	index_.resize((size_t) polygonCount_ * 3);

	for (int i = 0; i < materialCount_; i++) {
		for (DWORD poly = 0; poly < polygonCount_; poly++) {
			FbxLayerElementMaterial* materialLayer = mesh->GetLayer(0)->GetMaterials();
			int materialId = materialLayer->GetIndexArray().GetAt(poly);

			if (materialId == i) {
				for (DWORD vertex = 0; vertex < 3; vertex++) {
					index_[i].push_back(poly * 3 + vertex);
				}
			}

		}

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(int) * index_[i].size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = index_[i].data();				//インデックスバッファ

		HRESULT hr = GetDevice()->CreateBuffer(&bd, &data, &indexBuffer_[i]);
	}
}

void FBX::InitConstantBuffer() {
	pMaterialConstantBuffers_.resize(materialCount_);
	for (int i = 0; i < materialCount_; i++) {
		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		HRESULT hr = GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &pMaterialConstantBuffers_[i]);
	}
}

void FBX::InitMaterial(fbxsdk::FbxNode* node) {
	materials_.resize(materialCount_);	// vectorをmaterialcount分に変更

	for (int i = 0; i < materialCount_; i++) {	//マテリアル分ループする
		FbxSurfaceMaterial* material = node->GetMaterial(i);
		FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		int fileTextureCount = property.GetSrcObjectCount <FbxFileTexture>();
		FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)node->GetMaterial(i);
		FbxDouble3  diffuse = pMaterial->Diffuse;
		materials_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		FbxDouble3 ambient = pMaterial->Ambient;
		materials_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);

		if (fileTextureCount > 0) {	//　マテリアルにテクスチャがある場合
			FbxFileTexture* textureInfo = property.GetSrcObject<FbxFileTexture>();
			const char* texturePath = textureInfo->GetFileName();
			materials_[i].texture = new Texture(texturePath, -0.5f, 0.5f);	//　テクスチャデータを代入する
			materials_[i].texture->Init();
		}
		else {	// マテリアルにテクスチャがない場合
			materials_[i].texture = nullptr;	// 無いので、nullptrにする
		}
	}
}

void FBX::InitSkeleton(fbxsdk::FbxMesh* mesh) {
	FbxDeformer* pDeformer = mesh->GetDeformer(0);
	if (pDeformer == nullptr) return;
	pSkinInfo_ = (FbxSkin*)pDeformer;
	boneCount_ = pSkinInfo_->GetClusterCount();

	for (int i = 0; i < boneCount_; i++) {
		cluster_.push_back(pSkinInfo_->GetCluster(i));
	}

	weightList.resize(vertexCount_);
	for (DWORD i = 0; i < vertexCount_; i++)
	{
		weightList[i].posOrigin = vertices_[i].postion;
		//weightList[i].normalOrigin = vertices_[i].normal;
		weightList[i].boneIndex.resize(boneCount_);
		weightList[i].boneWeight.resize(boneCount_);
		for (int j = 0; j < boneCount_; j++)
		{
			weightList[i].boneIndex[j] = -1;
			weightList[i].boneWeight[j] = 0.0f;
		}
	}

	for (int i = 0; i < boneCount_; i++)
	{
		int numIndex = cluster_[i]->GetControlPointIndicesCount();   //このボーンに影響を受ける頂点数
		int* piIndex = cluster_[i]->GetControlPointIndices();       //ボーン/ウェイト情報の番号
		double* pdWeight = cluster_[i]->GetControlPointWeights();     //頂点ごとのウェイト情報

		for (int k = 0; k < numIndex; k++)
		{
			for (int m = 0; m < 4; m++)
			{
				if (m >= boneCount_)
					break;

				if (pdWeight[k] > weightList[piIndex[k]].boneWeight[m])
				{
					for (int n = boneCount_ - 1; n > m; n--)
					{
						weightList[piIndex[k]].boneIndex[n] = weightList[piIndex[k]].boneIndex[n - 1];
						weightList[piIndex[k]].boneWeight[n] = weightList[piIndex[k]].boneWeight[n - 1];
					}
					weightList[piIndex[k]].boneIndex[m] = i;
					weightList[piIndex[k]].boneWeight[m] = (float)pdWeight[k];
					break;
				}
			}

		}
	}

	boneList.resize(boneCount_);
	for (int i = 0; i < boneCount_; i++)
	{
		FbxAMatrix  matrix;
		cluster_[i]->GetTransformLinkMatrix(matrix);

		XMFLOAT4X4 pose;
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = (float)matrix.Get(x, y);
			}
		}
		boneList[i].bindPose = XMLoadFloat4x4(&pose);
		boneMap[cluster_[i]->GetLink()->GetName()] = &boneList[i];
	}
}

void FBX::Update() {
	auto currentCamera = CameraManager::getCurentCamera();
	XMMATRIX scaleMat = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	XMMATRIX rotMat = XMMatrixRotationZ(rotation_.z) * XMMatrixRotationX(rotation_.x) * XMMatrixRotationY(rotation_.y);
	XMMATRIX transMat = XMMatrixTranslation(postion_.x, postion_.y, postion_.z);
	world_ = scaleMat * rotMat * transMat;
	XMMATRIX view = currentCamera->getMatrix();
	XMMATRIX projection = currentCamera->GetProjection();

	for (int i = 0; i < materialCount_; i++) {
		ConstantBuffer cb = {};
		cb.wvpMat = XMMatrixTranspose(world_ * view * projection);
		cb.diffUse = materials_[i].diffuse;	// ディフューズカラーをコンスタントバッファに代入
		cb.ambient = materials_[i].ambient; // アンビエントカラーをコンスタントバッファに代入
		cb.speculer = materials_[i].specular; // スペキュラーをコンスタントバッファに代入
		cb.isTexture = materials_[i].texture != nullptr ? TRUE : FALSE; // テクスチャフラグをコンスタントバッファに代入
		cb.isMosaic = FALSE;
		GetContext()->UpdateSubresource(pMaterialConstantBuffers_[i], 0, nullptr, &cb, 0, 0); // コンスタントバッファを更新する
	}

}


void FBX::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

	ChangeDrawWireFrameMode(false);
	GetContext()->IASetInputLayout(ShaderManager::inputLayout_);
	GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GetContext()->VSSetShader(ShaderManager::vertexShader_, nullptr, 0);
	GetContext()->PSSetShader(ShaderManager::pixelShader_, nullptr, 0);
		
	for (int i = 0; i < materialCount_; i++) {
		GetContext()->IASetIndexBuffer(indexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);
		if (!isShowTexture_) {
			GetContext()->PSSetShaderResources(0, 1, nullSRV);
			GetContext()->PSSetShader(ShaderManager::pixelShader_, nullptr, 0);
		}
		else {
			if (materials_[i].texture != nullptr) {
				auto srv = (materials_[i].texture->GetShaderReasourceView());
				auto samplerState = materials_[i].texture->GetSampleState();
				GetContext()->PSSetShaderResources(0, 1, &srv);
				GetContext()->PSSetSamplers(0, 1, &samplerState);
			}
			else {
				GetContext()->PSSetShaderResources(0, 1, nullSRV);
			}
		}

		GetContext()->PSSetConstantBuffers(0, 1, &pMaterialConstantBuffers_[i]);
		GetContext()->VSSetConstantBuffers(0, 1, &pMaterialConstantBuffers_[i]);
		
		GetContext()->DrawIndexed(index_[i].size(), 0, 0);

		
	}
	GetContext()->RSSetState(nullptr);
}

void FBX::Release() {

}

bool FBX::GetBonePostion(const std::string& boneName, DirectX::XMFLOAT3* postion){
	for (int i = 0; i < boneCount_; i++) {	// ボーン分ループする
		if (boneName == cluster_[i]->GetLink()->GetName()) {	// 一致するボーンがあるか
			FbxAMatrix  matrix;
			cluster_[i]->GetTransformLinkMatrix(matrix);

			postion->x = (float)matrix[3][0];
			postion->y = (float)matrix[3][1];
			postion->z = (float)matrix[3][2];

			return true;
		}
	}

	return false;
}

bool FBX::Raycast(FBX* fbx, DirectX::XMFLOAT3 rayPos, DirectX::XMFLOAT3 rayDir, float& distance) {
	auto rayOrigin = DirectX::XMLoadFloat3(&rayPos);
	auto rayDirection = DirectX::XMLoadFloat3(&rayDir);

	bool hit = false;

	for (int v = 0; v < fbx->vertexCount_; v += 3) {
		auto vertex0 = DirectX::XMLoadFloat3(&fbx->vertices_[v].postion);
		auto vertex1 = DirectX::XMLoadFloat3(&fbx->vertices_[v + 1].postion);
		auto vertex2 = DirectX::XMLoadFloat3(&fbx->vertices_[v + 2].postion);

		vertex0 = XMVector3TransformCoord(vertex0, fbx->world_);
		vertex1 = XMVector3TransformCoord(vertex1, fbx->world_);
		vertex2 = XMVector3TransformCoord(vertex2, fbx->world_);

		float dis;
		if (DirectX::TriangleTests::Intersects(
			rayOrigin,
			rayDirection,
			vertex0,
			vertex1,
			vertex2,
			dis))
		{
			distance = dis;
			hit = true;
		}
	}

	return hit;
}
