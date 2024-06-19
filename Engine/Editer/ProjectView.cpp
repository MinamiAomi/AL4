#include "ProjectView.h"

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"
#include "Externals/ImGui/imgui_internal.h"
#endif // ENABLE_IMGUI

namespace Editer {

    void ProjectView::Render() {

#ifdef ENABLE_IMGUI
        if (!isDisplayed) { return; }

        ImGui::Begin("Project", &isDisplayed);


        ImGui::End();
#endif // ENABLE_IMGUI
    }

}