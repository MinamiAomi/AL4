#include "InspectorView.h"

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#endif // ENABLE_IMGUI


namespace Editer {

    void InspectorView::Render(const std::shared_ptr<SelectableInEditer>& selectableInEditer) {
        selectableInEditer;
#ifdef ENABLE_IMGUI
        ImGui::Begin("Inspector");
        if (selectableInEditer) {
            selectableInEditer->RenderInInspectorView();
        }
        else {
            ImGui::Text("Not selected!!");
        }
        ImGui::End();
#endif // ENABLE_IMGUI
    }

}