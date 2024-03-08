#include "TestObject.h"

#include "Graphics/ResourceManager.h"

#include "Graphics/ImGuiManager.h"

void TestObject::Initialize(const std::string& name, const Vector3& position) {
    model_ = std::make_unique<ModelInstance>();

    transform.translate = position;
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one;

    model_->SetModel(ResourceManager::GetInstance()->FindModel(name));
    model_->SetIsActive(true);
    //model_->SetReflection(true);
}

void TestObject::Update() {

    if (rotate_) {
        transform.rotate = Quaternion::MakeForYAxis(Math::ToRadian) * transform.rotate;
    }

    transform.UpdateMatrix();
    model_->SetWorldMatrix(transform.worldMatrix);
}

void TestObject::DrawImGui(const std::string& label) {
    (void)label;
#ifdef ENABLE_IMGUI
    auto model = model_->GetModel();
    assert(model);
    auto& mesh = model->GetMeshes()[0];
    assert(mesh.material);

    bool openTree = ImGui::TreeNode(label.c_str());
    if (openTree) {
        ImGui::ColorEdit3("Albedo", &mesh.material->albedo.x);
        ImGui::DragFloat("Metallic", &mesh.material->metallic, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Roughness", &mesh.material->roughness, 0.01f, 0.0f, 1.0f);
        ImGui::TreePop();
    }
#endif //ENABLE_IMGUI
}

