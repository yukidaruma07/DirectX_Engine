#pragma once
#include "BaseObject.h"
#include "FBXParent.h"
#include <unordered_map>

class FBXChildren : public BaseObject {
private:
	ID3D11Buffer* vertexBuffer_; //頂点バッファ
	std::vector<Vertex> vertices_; //頂点のデータ
	std::vector<ID3D11Buffer*> pMaterialConstantBuffers_;	// マテリアルごとのコンスタントバッファ
	std::vector<ID3D11Buffer*> indexBuffer_; //インデックスバッファ
	std::vector<std::vector<int>> index_;	// インデックスのデータ
	bool wireFrame_;
private:
	bool isShowTexture_;
	FBXLoadOption fbxLoadOption_;	//　FBXを初期化するためのオプション

	FbxNode* node_;
	FbxMesh* mesh_;

	int materialCount_;	// マテリアルの数
	int vertexCount_;	// 頂点の数
	int polygonCount_;	// ポリゴンの数
	int indexCount_;	// インデックスの数
	int boneCount_;		// ボーンの数
	std::vector<MATERIAL> materials_; //マテリアルのデータ
	std::vector<int> controlPointIndexOfVertex_;

	/// 
	/// ボーン
	/// 
	FbxSkin* pSkinInfo_;
	std::vector<FbxCluster*> cluster_;
	std::vector<Bone> boneList;
	std::unordered_map<std::string, Bone*> boneMap;
	std::vector<Weight> weightList;

	/// 
	/// アニメーション
	/// 
	bool isAnime;
	FbxTime time_ = {};
	float nowFrame, animSpeed;
	int startFrame, endFrame;
protected:
public:

	FBXChildren(FBXParent* parent, FbxNode* node);
	~FBXChildren();

	void Init() override;

	/// <summary>
/// 頂点を初期化する関数
/// </summary>
/// <param name="mesh">読み込むFBXのメッシュ</param>
	void InitVertex(fbxsdk::FbxMesh* mesh);

	/// <summary>
	/// インデックスバッファを初期化する関数
	/// </summary>
	/// <param name="mesh">読み込むFBXのメッシュ</param>
	void InitIndex(fbxsdk::FbxMesh* mesh);

	/// <summary>
	/// コンスタントバッファを初期化する関数
	/// </summary>
	void InitConstantBuffer();

	/// <summary>
	/// マテリアルを初期化する関数
	/// </summary>
	/// <param name="node">FBXのノード</param>
	void InitMaterial(fbxsdk::FbxNode* node);

	void InitSkeleton(fbxsdk::FbxMesh* mesh);

	void Update() override;
	void Draw() override;
	void Release() override;

	void DrawObjectInfoImGUI() override {
		ImGui::Begin("FBX");
		ImGui::SliderFloat("X", &transform_.postion_.x, -1280.0f, 1280.0f);
		ImGui::SliderFloat("Y", &transform_.postion_.y, -1280.0f, 1280.0f);
		ImGui::SliderFloat("Z", &transform_.postion_.z, -1280.0f, 1280.0f);
		ImGui::SliderFloat("angleX", &transform_.rotation_.x, 0.0f, 90.0f);
		ImGui::SliderFloat("angleY", &transform_.rotation_.y, 0.0f, 90.0f);
		ImGui::SliderFloat("angleZ", &transform_.rotation_.z, 0.0f, 90.0f);
		ImGui::SliderFloat("scaleX", &transform_.scale_.x, 0.0f, 10.0f);
		ImGui::SliderFloat("scaleY", &transform_.scale_.y, 0.0f, 10.0f);
		ImGui::SliderFloat("scaleZ", &transform_.scale_.z, 0.0f, 10.0f);
		ImGui::End();
	}

	/// <summary>
	/// ボーンを取得する関数
	/// </summary>
	/// <param name="boneName">取得したいボーンの名前</param>
	/// <param name="postion">返す用のボーンの位置</param>
	/// <returns>ボーンが見つかった場合はtrue、見つからなかった場合はfalseを返す。</returns>
	bool GetBonePostion(const std::string& boneName, DirectX::XMFLOAT3* postion);

	/// <summary>
	/// アニメーション付きのFBXを描画するための関数。
	/// </summary>
	void DrawAnime();

	/// <summary>
	/// レイキャストを行う関数
	/// </summary>
	bool Raycast(DirectX::XMFLOAT3 rayPos, DirectX::XMFLOAT3 rayDir, float& distance);

	bool IsWireframe() { return this->wireFrame_; }
	void EnableWireFrame() { this->wireFrame_ = true; }
	void DisableWireFrame() { this->wireFrame_ = false; }
};