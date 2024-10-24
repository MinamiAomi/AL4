#include "MeshComponent.h"

#include "GameObject/GameObject.h"

#include "Framework/AssetManager.h"
#include "Graphics/ImGuiManager.h"

void MeshComponent::Initialize() {
    auto assetManager = AssetManager::GetInstance();
    asset_ = assetManager->modelMap.Get(modelName_);
    ApplyModel();

    auto gameObject = GetGameObject();
    gameObject->transform.UpdateMatrix();
    model_.SetWorldMatrix(gameObject->transform.worldMatrix);

}

void MeshComponent::Update() {
    model_.SetWorldMatrix(GetGameObject()->transform.worldMatrix);
}

void MeshComponent::Edit() {
#ifdef ENABLE_IMGUI

    if (ImGui::BeginCombo("Model", asset_->GetName().c_str())) {
        AssetManager::GetInstance()->modelMap.ForEach([this](const std::shared_ptr<ModelAsset>& asset) {
            bool isSelected = asset->GetName() == asset_->GetName();
            if (ImGui::Selectable(asset->GetName().c_str(), isSelected) && asset->IsReady()) {
                asset_ = asset;
                ApplyModel();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
            });
        ImGui::EndCombo();
    }

    if (customMaterial_) {
        ImGui::ColorEdit3("Albedo", &customMaterial_->albedo.x);
        ImGui::SliderFloat("Metallic", &customMaterial_->metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &customMaterial_->roughness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Emissive", &customMaterial_->emissive.x);
        ImGui::DragFloat("EmissiveIntensity", &customMaterial_->emissiveIntensity, 0.1f);
    }
#endif // ENABLE_IMGUI
}

void MeshComponent::ApplyModel() {
    assert(asset_);
    model_.SetModel(asset_->Get());

    if (model_.GetModel()->GetMaterials().size() <= 2) {
        customMaterial_ = std::make_shared<Material>(model_.GetModel()->GetMaterials()[0]);
        model_.SetMaterial(customMaterial_);
    }
}
