#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include <fbxsdk.h>

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

class FBXParent : public BaseObject {
protected:
private:
	std::string fileName_;
	bool isShowTexture_;
	FBXLoadOption fbxLoadOption_;	//　FBXを初期化するためのオプション
	FbxManager* fbxManager_;
	FbxImporter* fbxImporter_;

	FbxNode* rootNode_;
public:

	FBXParent(const std::string fName, FBXLoadOption fbxLoadOption);
	~FBXParent();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;

};