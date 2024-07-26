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

    enum class Filter {
        Texture,
        Model,
        Animation,
        Sound
    };

    std::wstring OpenFileDialog(const std::wstring& title, Filter filter) {
        OPENFILENAME ofn{};
        wchar_t szFile[256]{};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = Engine::GetGameWindow()->GetHWND();
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = sizeof(szFile);
        switch (filter)
        {
        case Filter::Texture:
            ofn.lpstrFilter = L"Texture\0*.PNG;*.JPG;*.DDS;*.HDR";
            break;
        case Filter::Model:
            ofn.lpstrFilter = L"Model\0*.OBJ;*.GLTF";
            break;
        case Filter::Animation:
            ofn.lpstrFilter = L"Animation\0*.GLTF";
            break;
        case Filter::Sound:
            ofn.lpstrFilter = L"Sound\0*.MP3;*.WAV";
            break;
        }
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

    std::string TruncateText(const std::string& text, float maxWidth) {
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        if (textSize.x <= maxWidth) {
            return text;
        }

        std::string truncatedText = text;
        while (truncatedText.size() > 0 && ImGui::CalcTextSize((truncatedText + "...").c_str()).x > maxWidth) {
            truncatedText.pop_back();
        }

        return truncatedText + "...";
    }

    ImVec2 CalcFitSize(const ImVec2& target, const ImVec2& size) {
        float aspect = size.x / size.y;
        if (aspect > 1.0f) {
            return { target.x, target.y * (1.0f / aspect) };
        }
        return { target.x * aspect, target.y };
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
                if (ImGui::BeginMenu("Import")) {
                    if (ImGui::MenuItem("Texture")) {
                        auto path = OpenFileDialog(L"Texture import", Filter::Texture);
                        path;
                    }
                    if (ImGui::MenuItem("Model")) {
                        auto path = OpenFileDialog(L"Model import", Filter::Model);
                        path;
                    }
                    if (ImGui::MenuItem("Animation")) {
                        auto path = OpenFileDialog(L"Animation import", Filter::Animation);
                        path;
                    }
                    if (ImGui::MenuItem("Sound")) {
                        auto path = OpenFileDialog(L"Sound import", Filter::Sound);
                        path;
                    }
                    ImGui::EndMenu();
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

        ImVec2 windowSize = ImGui::GetWindowSize();

        const int32_t numItems = 10;  // 表示するアイテムの数
        const float itemSize = 64.0f;  // テクスチャのサイズ
//        const float itemSpacing = 20.0f;  // アイテム間のスペース

        for (int32_t i = 0; i < numItems; ++i) {

            ImGui::BeginGroup();
            //ImGui::Button(std::format("##{}", i).c_str(), CalcFitSize({ itemSize, itemSize }, {16.0f * i, 64.0f}));
            ImGui::Button(std::format("##{}", i).c_str(), {64.0f, 64.0f});
            // テキストの描画
            std::string text = "Item " + std::to_string(i);
            text = TruncateText(text, itemSize);
            ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
            float textOffsetX = (itemSize - textSize.x) / 2.0f;  // 中央揃えのためのオフセット計算
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
            if(i == 0) 
                ImGui::SetCursorPosY( itemSize + ImGui::GetStyle().ItemSpacing.y * 2.0f);
            ImGui::Text("%s", text.c_str());
            ImGui::EndGroup();

            ImGui::SameLine();
        }
    }

}