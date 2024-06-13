#include "HierarchyView.h"

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"
#endif // ENABLE_IMGUI

#include "GameObject/GameObjectManager.h"
#include "GameObject/GameObject.h"
#include "EditerManager.h"

namespace Editer {

    void HierarchyView::Render(const GameObjectManager& gameObjectManager) {
        gameObjectManager;
#ifdef ENABLE_IMGUI
        ImGui::Begin("Hierarchy");
        ImGui::InputText("##", &searchString_);

        const auto& gameObjects = gameObjectManager.GetGameObjects();
        for (auto& gameObject : gameObjects) {
            // 親がいないオブジェクトから再帰的に
            if (!gameObject->HasParent()) {
                RenderGameObject(gameObject, 0);
            }
        }
        ImGui::End();
#endif // ENABLE_IMGUI
    }

    void HierarchyView::RenderGameObject(const std::shared_ptr<GameObject>& gameObject, int32_t depth) {
        static const float kIndentWidth = 20;
        ImGui::PushID(gameObject.get());
        // depthに応じてインデントを増やす
        ImGui::Indent(depth * kIndentWidth);
        // 選択可能にする
        if (ImGui::Selectable(gameObject->GetName().c_str(), gameObject == owner.GetSelectedObject())) {
            owner.SetSelectedObject( gameObject);
        }
        ImGui::Unindent();
        ImGui::PopID();
        // 子の分だけ再帰
        for (auto& child : gameObject->GetChildren()) {
            if (auto sp = child.lock()) {
                RenderGameObject(sp, depth + 1);
            }
        }
    }

}
