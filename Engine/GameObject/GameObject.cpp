#include "GameObject.h"

#include "Graphics/ImGuiManager.h"

void GameObject::InitializeUninitializedComponents() {
    // 未初期化のコンポーネントを初期化
    for (auto component : uninitializedComponents_) {
        component->Initialize();
    }
    uninitializedComponents_.clear();
}

void GameObject::Update() {
    // すべてのコンポーネントを更新
    for (auto& component : componentList_) {
        component.second->Update();
    }
    transform.UpdateMatrix();
}

void GameObject::ImguiDraw() {
    
    ImGui::PushID(this);
    ImGui::Text(name_.c_str());
    if (ImGui::TreeNode("Transform")) {
        ImGui::Separator();
        ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
        Vector3 rotate = transform.rotate.EulerAngle() * Math::ToDegree;
        ImGui::DragFloat3("Rotate", &rotate.x, 1.0f);
        transform.rotate = Quaternion::MakeFromEulerAngle(rotate * Math::ToRadian);
        ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        transform.UpdateMatrix();
        ImGui::TreePop();
    }
    ImGui::PopID();
}
