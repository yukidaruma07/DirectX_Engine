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

enum class FBXPostionType {
	LEFTX_YUP_DEPTHZ,		// 左右をX、上下をY、深さをZ（Mayaでの推奨）
	LEFTX_ZUP_DEPTHY		// 左右をX、上下をZ、深さをY（Blenderでの推奨）
};

/// <summary>
/// FBXの読み込むオプション
/// </summary>
struct FBXLoadOption {
	FBXPostionType postionType = FBXPostionType::LEFTX_YUP_DEPTHZ;
};

/// <summary>
/// マテリアルの構造体
/// </summary>
struct MATERIAL {
	Texture* texture;			// テクスチャのデータ
	DirectX::XMFLOAT4 diffuse;	// ディフューズ（マテリアルの色）
	DirectX::XMFLOAT4 ambient;  // 環境光（影の色）
	DirectX::XMFLOAT4 specular; // 光沢（反射した時の色）
	float shininess;			// 輝きの強さ
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
	ID3D11Buffer* vertexBuffer_; //頂点バッファ
	std::vector<Vertex> vertices_; //頂点のデータ
	std::vector<ID3D11Buffer*> pMaterialConstantBuffers_;	// マテリアルごとのコンスタントバッファ
	std::vector<ID3D11Buffer*> indexBuffer_; //インデックスバッファ
	std::vector<std::vector<int>> index_;	// インデックスのデータ
private:

	/// 
	/// FBXの基本データ
	/// 
	std::string path_;		// FBXが存在するパス
	bool isShowTexture_;	// テクスチャを表示するかのフラグ
	FBXLoadOption fbxLoadOption_;	//　FBXを初期化するためのオプション
	FbxManager* fbxManager_;
	FbxImporter* fbxImporter_;
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

public:
	DirectX::XMMATRIX world_;

	FBX(const std::string fName, FBXLoadOption fbxLoadOption);
	~FBX();

	ID3D11Buffer* GetVertexBuffer() const { return vertexBuffer_; }
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
	static bool Raycast(FBX* fbx, DirectX::XMFLOAT3 rayPos, DirectX::XMFLOAT3 rayDir, float& distance);
};

