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

    void Splitter(bool split_vertically, float thickness, float& size1, float& size2, float min_size1, float min_size2)
    {
        ImVec2 backup_pos = ImGui::GetCursorPos();
        if (split_vertically)
            ImGui::SetCursorPosY(backup_pos.y + size1);
        else
            ImGui::SetCursorPosX(backup_pos.x + size1);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::Button("##Splitter", ImVec2(split_vertically ? -1 : thickness, split_vertically ? thickness : -1));
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        if (ImGui::IsItemActive())
        {
            if (split_vertically)
            {
                float mouse_delta = ImGui::GetIO().MouseDelta.y;
                if (mouse_delta != 0.0f)
                {
                    if (mouse_delta < min_size1 - size1) mouse_delta = min_size1 - size1;
                    if (mouse_delta > size2 - min_size2) mouse_delta = size2 - min_size2;
                    size1 += mouse_delta;
                    size2 -= mouse_delta;
                }
            }
            else
            {
                float mouse_delta = ImGui::GetIO().MouseDelta.x;
                if (mouse_delta != 0.0f)
                {
                    if (mouse_delta < min_size1 - size1) mouse_delta = min_size1 - size1;
                    if (mouse_delta > size2 - min_size2) mouse_delta = size2 - min_size2;
                    size1 += mouse_delta;
                    size2 -= mouse_delta;
                }
            }
        }

        ImGui::SetCursorPos(backup_pos);
    }

}

namespace Editer {

    void ProjectView::Render() {

#ifdef ENABLE_IMGUI
        if (!isDisplayed) { return; }

        if (ImGui::Begin("Project", &isDisplayed, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {



            if (ImGui::BeginMenuBar()) {
                if (ImGui::MenuItem("Import")) {
                    auto path = OpenFileDialog(L"Import");
                    Debug::Log(std::format(L"{} : {} open\n", path, path.size()));
                }
                ImGui::EndMenuBar();
            }


            auto windowSize = ImGui::GetWindowSize();
            const float splitterWidth = 2.0f;
            float childPosY = ImGui::GetFrameHeight() * 2.0f;

            // 子ウィンドウの上部の高さ
            ImGui::SetCursorPos({ 0.0f, childPosY });
            // 左ウィンドウ]
            ImGui::BeginChild("Left", { leftWindowSize_, windowSize.y - childPosY }, ImGuiChildFlags_AlwaysUseWindowPadding);
            ImGui::Text("text");

            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SetCursorPos({ leftWindowSize_ + splitterWidth, childPosY });
            ImGui::BeginChild("Right", { windowSize.x - leftWindowSize_ - splitterWidth, windowSize.y - childPosY }, ImGuiChildFlags_AlwaysUseWindowPadding);
            ImGui::Text("text");



            ImGui::EndChild();




            ImGui::SameLine();
            ImGui::SetCursorPos({ leftWindowSize_, childPosY });
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Border));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered));
            ImGui::Button("##S", { splitterWidth, windowSize.y - childPosY });
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();
            if (ImGui::IsItemHovered()) { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); }
            if (ImGui::IsItemActive()) {
                const float minWidth = 100.0f;
                float mouseDelta = ImGui::GetIO().MouseDelta.x;
                leftWindowSize_ += mouseDelta;
                if (leftWindowSize_ < minWidth) { leftWindowSize_ = minWidth; }
                if (windowSize.x - leftWindowSize_ - splitterWidth < minWidth) { leftWindowSize_ = windowSize.x - minWidth - splitterWidth; }
            }

        }
        ImGui::End();


#endif // ENABLE_IMGUI
    }

}