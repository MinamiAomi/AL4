#include "EditerManager.h"

#include <Windows.h>
#include <d3d12.h>

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#endif // ENABLE_IMGUI


#include "Framework/Engine.h"
#include "Graphics/Core/Graphics.h"
#include "Graphics/GameWindow.h"
#include "Graphics/RenderManager.h"

namespace Editer {

    EditerManager::EditerManager() :
        hierarchyView_(std::make_unique<HierarchyView>(*this)),
        inspectorView_(std::make_unique<InspectorView>(*this)),
        sceneView_(std::make_unique<SceneView>(*this)),
        debugView_(std::make_unique<DebugView>(*this)) {
    }

    void EditerManager::Initialize() {
        auto graphics = Engine::GetGraphics();
        auto window = Engine::GetGameWindow();
        auto renderManager = Engine::GetRenderManager();

        descriptor_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX12_Init(
            graphics->GetDevice(),
            SwapChain::kNumBuffers,
            renderManager->GetSwapChain().GetColorBuffer(0).GetRTVFormat(),
            graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
            descriptor_,
            descriptor_);
    }

    void EditerManager::Render() {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();

        RenderDockingSpace();

        auto gameObjectManager = Engine::GetGameObjectManager();
        hierarchyView_->Render(*gameObjectManager);
        inspectorView_->Render(selectedObject_);

    }

    void EditerManager::RenderToColorBuffer(CommandContext& commandContext) {
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContext);
    }

    void EditerManager::Finalize() {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void EditerManager::RenderDockingSpace() {
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // ウィンドウフラグの設定
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // メインドッキングスペースの開始
        ImGui::Begin("DockSpace", nullptr, windowFlags);

        ImGui::PopStyleVar(2);

        // ドッキングスペースの設定
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspaceID = ImGui::GetID("Dockspace");
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }

        // メニューバーの例
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Exit", NULL, false, false); // 無効なメニューアイテム
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

}