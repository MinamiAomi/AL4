#include "SponzaScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Scene/SceneManager.h"
#include "Graphics/ImGuiManager.h"

void SponzaScene::OnInitialize() {
    debugCamera_ = std::make_shared<DebugCamera>();
    debugCamera_->Initialize();

    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);
    sunLight_->direction = Vector3(0.0f, -1.0f, 0.001f).Normalized();

    testObjects_.emplace_back(std::make_shared<TestObject>());
    testObjects_.back()->Initialize("Sponza", {});
    testObjects_.back()->transform.scale = { 0.05f, 0.05f, 0.05f };
    testObjects_.back()->SetRotate(false);


}

void SponzaScene::OnUpdate() {

    for (auto& testObject : testObjects_) {
        testObject->Update();
    }

    debugCamera_->Update();

#ifdef _DEBUG
    if (ImGui::TreeNode("SunLight")) {
        ImGui::DragFloat3("Direction", &sunLight_->direction.x, 0.01f, -1.0f, 1.0f);
        sunLight_->direction = sunLight_->direction.Normalized();
        ImGui::ColorEdit3("Color", &sunLight_->color.x);
        ImGui::DragFloat("Intensity", &sunLight_->intensity, 0.01f);
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void SponzaScene::OnFinalize() {

}
