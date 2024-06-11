#include "GameObjectManager.h"

#include "Graphics/ImGuiManager.h"

void GameObjectManager::Update() {
    for (auto& gameObject : gameObjects_) {
        gameObject->InitializeUninitializedComponents();
    }
    for (auto& gameObject : gameObjects_) {
        gameObject->Update();
    }
}

void GameObjectManager::ImGuiDraw() {
#ifdef ENABLE_IMGUI
    ImGui::Begin("Objects");
    for (auto& gameObject : gameObjects_) {
        ImGui::PushID(gameObject.get());
        ImGui::Checkbox("##", &gameObject->isActive_);
        ImGui::SameLine();
        if (ImGui::Selectable(gameObject->name_.c_str(), gameObject == selectedGameObject_)) {
            selectedGameObject_ = gameObject;
        }
        ImGui::PopID();
    }
    ImGui::End();

    ImGui::Begin("Inspector");
    if (selectedGameObject_ != nullptr) {
        selectedGameObject_->ImguiDraw();
    }
    ImGui::End();
#endif // ENABLE_IMGUI
}
