#include "ProjectView.h"

#include <Windows.h>

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_stdlib.h"
#include "Externals/ImGui/imgui_internal.h"
#endif // ENABLE_IMGUI

#include "FrameWork/Engine.h"
#include "Graphics/GameWindow.h"
#include "Debug/Debug.h"

namespace {

    std::wstring OpenFileDialog(const std::wstring& title) {
        OPENFILENAME ofn{};
        wchar_t szFile[256]{};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = Engine::GetGameWindow()->GetHWND();
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All\0*.*\0Texture\0*.PNG;*.JPG;*.DDS;*.HDR;\0Model\0*.OBJ;*.GLTF";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrTitle = title.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE) {
            return std::wstring(ofn.lpstrFile);
        }
        return std::wstring();
    }

}

namespace Editer {

    void ProjectView::Render() {

#ifdef ENABLE_IMGUI
        if (!isDisplayed) { return; }

        ImGui::Begin("Project", &isDisplayed, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Import")) {
                auto path = OpenFileDialog(L"Import");
                Debug::Log(std::format(L"{} : {} open\n", path, path.size()));
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
#endif // ENABLE_IMGUI
    }

}