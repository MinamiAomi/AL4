#include "Ground.h"

#include "Graphics/ResourceManager.h"
#include "Graphics/RenderManager.h"

#include "Graphics/ImGuiManager.h"

void Ground::Initialize() {
    model_ = std::make_unique<ModelInstance>();
    skydomeModel_ = std::make_unique<ModelInstance>();
    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);
    sunLight_->direction = Vector3(0.0f, -1.0f, -1.0f).Normalized();

    model_->SetModel(ResourceManager::GetInstance()->FindModel("Floor"));
    model_->SetReflection(true);

    skydomeModel_->SetModel(ResourceManager::GetInstance()->FindModel("skydome"));
    skydomeModel_->SetUseLighting(false);
    skydomeModel_->SetCastShadow(false);
    skydomeModel_->SetReciveShadow(false);
    skydomeModel_->SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, -10.0f, 0.0f }));
}

void Ground::Update() {

    transform.UpdateMatrix();
    model_->SetWorldMatrix(transform.worldMatrix);

#ifdef ENABLE_IMGUI
    if (ImGui::TreeNode("SunLight")) {
        ImGui::DragFloat3("Direction", &sunLight_->direction.x, 0.01f, -1.0f, 1.0f);
        sunLight_->direction = sunLight_->direction.Normalized();
        ImGui::ColorEdit3("Color", &sunLight_->color.x);
        ImGui::DragFloat("Intensity", &sunLight_->intensity, 0.01f);
        ImGui::TreePop();
    }
#endif // ENABLE_IMGUI
}