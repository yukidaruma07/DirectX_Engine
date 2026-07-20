#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include "../ImGUI/imgui.h"

class CircleCollider;

/// <summary>
/// オブジェクトの基底クラス
/// </summary>
class BaseObject {
private:
protected:
    std::vector<CircleCollider*> colliderList;  // オブジェクトが持っているコライダー
    std::string name_;                          // オブジェクトの名前
    bool isDead_;                               // オブジェクトが死んでいるか
    bool isShowImGUI_;                          // ImGUIを表示するかのフラグ

    DirectX::XMFLOAT3 postion_;                 // オブジェクトの場所
    DirectX::XMFLOAT3 velocity_;                // オブジェクトのベクトル
    DirectX::XMFLOAT3 rotation_;                // オブジェクトの回転
    DirectX::XMFLOAT3 scale_;                   // オブジェクトの大きさ
public:

    BaseObject(const std::string& name, const bool isDead = false) {
        name_ = name;
        isDead_ = isDead;

        postion_ = { 0.0f, 0.0f, 0.0f };
        velocity_ = { 0.0f, 0.0f, 0.0f };
        rotation_ = { 0.0f, 0.0f, 0.0f };
        scale_ = { 1.0f, 1.0f, 1.0f };

        isShowImGUI_ = true;
    };
    virtual ~BaseObject() {}

    /// <summary>
    /// オブジェクトの状態を初期化する関数（※実装必須）
    /// </summary>
    virtual void Init() = 0;

    /// <summary>
    /// オブジェクトの状態を更新する関数（※実装必須）
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// オブジェクトを描画する関数（※実装必須）
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// オブジェクトのデータを開放する関数（※実装必須）
    /// </summary>
    virtual void Release() = 0;

    /// <summary>
    /// オブジェクトの情報をImGUIで描画する関数
    /// </summary>
    virtual void DrawObjectInfoImGUI() {
        ImGui::Begin(name_.c_str());
        ImGui::SliderFloat("X", &postion_.x, -1280.0f, 1280.0f);
        ImGui::SliderFloat("Y", &postion_.y, -1280.0f, 1280.0f);
        ImGui::SliderFloat("Z", &postion_.z, -1280.0f, 1280.0f);
        ImGui::SliderFloat("angleX", &rotation_.x, 0.0f, 90.0f);
        ImGui::SliderFloat("angleY", &rotation_.y, 0.0f, 90.0f);
        ImGui::SliderFloat("angleZ", &rotation_.z, 0.0f, 90.0f);
        ImGui::SliderFloat("scaleX", &scale_.x, 0.0f, 10.0f);
        ImGui::SliderFloat("scaleY", &scale_.y, 0.0f, 10.0f);
        ImGui::SliderFloat("scaleZ", &scale_.z, 0.0f, 10.0f);
        ImGui::End();
    }

    /// <summary>
    /// 何かと当たったかを検出する関数
    /// </summary>
    /// <param name="obj">当たったオブジェクト</param>
    virtual void OnCollide(BaseObject* obj) {}

    /// <summary>
    /// オブジェクトの名前を返す関数
    /// </summary>
    std::string GetName() { return name_; };

    /// <summary>
    /// オブジェクトが死んでいるかを返す関数。
    /// </summary>
    /// <returns>死んでいる場合はtrue、死んでいない場合はfalse。</returns>
    bool IsDead() { return isDead_; };

    /// <summary>
    /// オブジェクトをキルする関数。
    /// </summary>
    void KillMe() { this->isDead_ = true; }

    /// <summary>
    /// ImGUIが表示させる設定化を返す関数
    /// </summary>
    bool IsShowImGUI() { return isShowImGUI_; }

    /// <summary>
    /// ImGUIを表示させるか設定する関数
    /// </summary>
    void SetShowImGUI(bool flag) { this->isShowImGUI_ = flag; }

    DirectX::XMFLOAT3 GetPosition() const { return postion_; }
    void SetPosition(const DirectX::XMFLOAT3 position) { this->postion_ = position; }
    DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    void SetVelocity(const DirectX::XMFLOAT3 velocity) { this->velocity_ = velocity; }
    DirectX::XMFLOAT3 GetRotation() const { return rotation_; }
    void SetRotation(const DirectX::XMFLOAT3 rotation) { this->rotation_ = rotation; }
    DirectX::XMFLOAT3 GetScale() const { return scale_; }
    void SetScale(const DirectX::XMFLOAT3 scale) { this->scale_ = scale; }

    std::vector<CircleCollider*>& GetColiderList() { return colliderList; }
};