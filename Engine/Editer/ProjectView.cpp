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

        if (ImGui::Begin("Project", &isDisplayed, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {


            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup("ProjectPopup");
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::MenuItem("Import")) {
                    auto path = OpenFileDialog(L"Import");
                    Debug::Log(std::format(L"{} : {} open\n", path, path.size()));
                }
                ImGui::EndMenuBar();
            }

            ImVec2 windowSize = ImGui::GetWindowSize();
            float childPosY = ImGui::GetFrameHeight() * 2.0f;

            // 子ウィンドウの上部の高さ
            ImGui::SetCursorPos({ 0.0f, childPosY });
            // 左ウィンドウ
            ImGui::BeginChild("Left", { leftWindowSize_, windowSize.y - childPosY }, ImGuiChildFlags_ResizeX);
            ImGui::SetCursorPos(ImGui::GetStyle().WindowPadding);
            leftWindowSize_ = ImGui::GetWindowWidth();
            RenderLeftWindow();
            ImGui::EndChild();


            ImGui::SameLine();
            ImGui::SetCursorPos({ leftWindowSize_, childPosY });
            ImGui::BeginChild("Right", { windowSize.x - leftWindowSize_, windowSize.y - childPosY }, ImGuiChildFlags_AlwaysUseWindowPadding);
            RenderRightWindow();
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SetCursorPos({ leftWindowSize_ - 2.0f, childPosY });
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            ImGui::Button("##Border20240725", { 2.0f, windowSize.y - childPosY });
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();


            if (ImGui::BeginPopup("ProjectPopup")) {
                ImGui::Text("popup");

                ImGui::EndPopup();
            }
        }
        ImGui::End();





#endif // ENABLE_IMGUI
    }

    void ProjectView::RenderLeftWindow() {
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if (ImGui::TreeNodeEx("Assets", flags)) {
            if (ImGui::TreeNodeEx("Textures", flags | ImGuiTreeNodeFlags_Leaf)) {

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Models", flags | ImGuiTreeNodeFlags_Leaf)) {

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Materials", flags | ImGuiTreeNodeFlags_Leaf)) {

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Animations", flags | ImGuiTreeNodeFlags_Leaf)) {
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Sounds", flags | ImGuiTreeNodeFlags_Leaf)) {

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
      


    }

    void ProjectView::RenderRightWindow() {
    }

}