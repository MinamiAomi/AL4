#include "ConsoleView.h"

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"
#include "Externals/ImGui/imgui_internal.h"
#endif // ENABLE_IMGUI

namespace Editer {

    void ConsoleView::Render() {

#ifdef ENABLE_IMGUI
        if (!isDisplayed) { return; }

        ImGui::Begin("Console", &isDisplayed);
        

        ImGui::End();
#endif // ENABLE_IMGUI
    }

}