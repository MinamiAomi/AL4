#include "MeshComponent.h"

#include "GameObject/GameObject.h"

#include "Framework/AssetManager.h"
#include "Graphics/ImGuiManager.h"

void MeshComponent::Initialize() {
    auto assetManager = AssetManager::GetInstance();
    model_.SetModel(assetManager->FindModel(modelName_));
    
    customMaterial_ = std::make_shared<Material>(model_.GetModel()->GetMaterials()[0]);
    model_.SetMaterial(customMaterial_);

    auto gameObject = GetGameObject();
    gameObject->transform.UpdateMatrix();
    model_.SetWorldMatrix(gameObject->transform.worldMatrix);

}

void MeshComponent::Update() {
    model_.SetWorldMatrix(GetGameObject()->transform.worldMatrix);
}

void MeshComponent::Edit() {
#ifdef ENABLE_IMGUI
    ImGui::ColorEdit3("Albedo", &customMaterial_->albedo.x);
    ImGui::SliderFloat("Metallic", &customMaterial_->metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &customMaterial_->roughness, 0.0f, 1.0f);
    ImGui::ColorEdit3("Emissive", &customMaterial_->emissive.x);
    ImGui::DragFloat("EmissiveIntensity", &customMaterial_->emissiveIntensity, 0.1f);
#endif // ENABLE_IMGUI
}
