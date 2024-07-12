#include "MeshComponent.h"

#include "GameObject/GameObject.h"

#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"

void MeshComponent::Initialize() {
    auto resourceManager = ResourceManager::GetInstance();
    model_.SetModel(resourceManager->FindModel(modelName_));
    
    customMaterial_ = std::make_shared<PBRMaterial>(model_.GetModel()->GetMaterials()[0]);
    model_.SetMaterial(customMaterial_);

    auto gameObject = GetGameObject();
    gameObject->transform.UpdateMatrix();
    model_.SetWorldMatrix(gameObject->transform.worldMatrix);

}

void MeshComponent::Update() {
    model_.SetWorldMatrix(GetGameObject()->transform.worldMatrix);
}

void MeshComponent::Edit() {
    ImGui::ColorEdit3("Albedo", &customMaterial_->albedo.x);
    ImGui::SliderFloat("Metallic", &customMaterial_->metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &customMaterial_->roughness, 0.0f, 1.0f);
    ImGui::ColorEdit3("Emissive", &customMaterial_->emissive.x);
}
