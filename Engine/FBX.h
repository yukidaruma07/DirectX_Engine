#pragma once
#include <Windows.h>
#include "BaseObject.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include "fbxsdk.h"
#include "Texture.h"
#include <unordered_map>
#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

enum FBXPostionType {
	FBX_LEFTX_YUP_DEPTHX,	// 左右をX、上下をY、深さをZ（Maya）
	FBX_LEFTX_ZUP_DEPTHY		// 左右をX、上下をZ、深さをY（Blender）
};

struct FBXLoadOption {
	FBXPostionType postionType = FBX_LEFTX_YUP_DEPTHX;
};

struct MATERIAL {
	Texture* texture;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;
	float shininess;
};

struct Bone {
	DirectX::XMMATRIX bindPose = {};
	DirectX::XMMATRIX newPose = {};
	DirectX::XMMATRIX diffPose = {};
};

struct Weight {
	DirectX::XMFLOAT3 posOrigin = {};
	DirectX::XMFLOAT3 normalOrigin = {};
	std::vector<int> boneIndex;
	std::vector<float> boneWeight;
};

/// <summary>
/// FBXの3Dモデルを表示するクラス
/// </summary>
class FBX : public BaseObject {
private:
	bool isShowTexture_;
	std::string path_;

	FBXLoadOption fbxLoadOption_;
	ID3D11Buffer* pVertexBuffer_; //頂点バッファ
	std::vector<ID3D11Buffer*> indexBuffer_; //インデックスバッファ（※数は分からないため、ポインタの配列

	FbxManager* fbxManager_;
	FbxImporter* fbxImporter_;
	FbxSkin* pSkinInfo_;
	std::vector<std::vector<int>> index_;
	std::vector<ID3D11Buffer*> pMaterialConstantBuffers_;

	int materialCount_;
	int vertexCount_;
	int polygonCount_;
	int indexCount_;
	int boneCount_;
	std::vector<MATERIAL> materials_; //マテリアルの配列データ
	std::vector<Vertex> vertices_; //頂点の配列データ
	std::vector<FbxCluster*> cluster_;

	std::vector<Bone> boneList;
	std::unordered_map<std::string, Bone*> boneMap;
	std::vector<Weight> weightList;

	float nowFrame, animSpeed;
	int startFrame, endFrame;

	bool isAnime;
	std::vector<int> controlPointIndexOfVertex_;

	FbxTime time_ = {};
public:
	DirectX::XMMATRIX world_;

	FBX(const std::string fName, FBXLoadOption fbxLoadOption);
	~FBX();

	ID3D11Buffer* GetVertexBuffer() const { return pVertexBuffer_; }
	std::vector<ID3D11Buffer*> GetIndexBuffer() const { return indexBuffer_; }

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

	bool GetBonePostion(const std::string& boneName, DirectX::XMFLOAT3* postion);

	void DrawAnime();

	/// <summary>
	/// レイキャストを行う関数
	/// </summary>
	static bool Raycast(FBX* fbx, DirectX::XMFLOAT3 rayPos, DirectX::XMFLOAT3 rayDir, float& distance);
};

