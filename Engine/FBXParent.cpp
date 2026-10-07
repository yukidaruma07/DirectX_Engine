#include "FBXParent.h"
#include "FBXChildren.h"
#include <filesystem>

FBXParent::FBXParent(const std::string fileName, FBXLoadOption fbxLoadOption)
	: BaseObject("FBXParent") {
	fileName_ = fileName;
	this->fbxLoadOption_ = fbxLoadOption;
}

FBXParent::~FBXParent()
{
}

void FBXParent::Init() {
	if (!std::filesystem::exists(fileName_.c_str())) {
		MessageBox(NULL, "FBXファイルが存在しません。", NULL, MB_OK);
		return;
	}
	if (!fileName_.ends_with(".fbx")) {
		MessageBox(NULL, "FBXファイル以外を開くことはできません。", NULL, MB_OK);
		return;
	}

	fbxManager_ = FbxManager::Create();
	fbxImporter_ = FbxImporter::Create(fbxManager_, "imp");
	fbxImporter_->Initialize(fileName_.c_str(), -1, fbxManager_->GetIOSettings());
	FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxscene");
	fbxImporter_->Import(fbxScene);

	FbxGeometryConverter converter(fbxManager_);
	converter.Triangulate(fbxScene, true);	// fbxを三角化する

	rootNode_ = fbxScene->GetRootNode();
	for (int i = 0; i < rootNode_->GetChildCount(); i++) {
		FbxNode* childNode = rootNode_->GetChild(i);
		FbxMesh* childMesh = childNode->GetMesh();
		if (childMesh != nullptr) {
			FBXChildren* children = new FBXChildren(this, childNode);
			children->Init();
			childList_.push_back(children);
		}
	}

}

void FBXParent::Update() {

}

void FBXParent::Draw() {


}

void FBXParent::Release()
{
}
