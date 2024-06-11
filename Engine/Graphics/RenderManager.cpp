#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"

static bool useGrayscale = false;

namespace {
#ifdef ENABLE_IMGUI
    ImVec2 CalcAspectFitSize(const ImVec2& windowSize, const ImVec2& imageSize) {
        float windowAspect = windowSize.x / windowSize.y;
        float imageAspect = imageSize.x / imageSize.y;

        ImVec2 newSize;
        if (windowAspect > imageAspect) {
            // ウィンドウがよりワイドの場合、高さに合わせて幅を計算
            newSize.y = windowSize.y;
            newSize.x = imageSize.x * (windowSize.y / imageSize.y);
        }
        else {
            // ウィンドウがよりタイトの場合、幅に合わせて高さを計算
            newSize.x = windowSize.x;
            newSize.y = imageSize.y * (windowSize.x / imageSize.x);
        }
        return newSize;
    }
#endif // ENABLE_IMGUI
}

RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {
    graphics_ = Graphics::GetInstance();

    auto shaderManager = ShaderManager::GetInstance();
    shaderManager->Initialize();
    shaderManager->SetDirectory(std::filesystem::current_path() / SHADER_DIRECTORY);

    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    DefaultTexture::Initialize();

    auto& swapChainBuffer = swapChain_.GetColorBuffer(0);
    finalImageBuffer_.Create(L"FinalImageBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), swapChainBuffer.GetFormat(), swapChainBuffer.IsSRGB());

    skinningManager_.Initialize();
    geometryRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    lightingRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    skybox_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat(), geometryRenderingPass_.GetDepth().GetFormat());
    lineDrawer_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat());

    bloom_.Initialize(&lightingRenderingPass_.GetResult());
    fxaa_.Initialize(&bloom_.GetResult());
    postEffect_.Initialize(finalImageBuffer_);

    //    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    transition_.Initialize();
    raytracingRenderer_.Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    //raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    //computeShaderTester_.Initialize(1024, 1024);
    //commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //computeShaderTester_.Dispatch(commandContext_);
    //commandContext_.Finish(true);

    timer_.Initialize();

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHWND(), swapChainBuffer.GetRTVFormat());
    imguiManager->NewFrame();

    frameCount_ = 0;
}

void RenderManager::Finalize() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();

    DefaultTexture::Finalize();
}

void RenderManager::Render() {

    uint32_t targetSwapChainBufferIndex = (swapChain_.GetCurrentBackBufferIndex() + 1) % SwapChain::kNumBuffers;

    auto camera = camera_.lock();
    auto sunLight = sunLight_.lock();

    commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    skinningManager_.Update(commandContext_);

    if (camera && sunLight) {
        // 影、スペキュラ
        modelSorter_.Sort(*camera);;
        //    raytracingRenderer_.Render(commandContext_, *camera, *sunLight);
        geometryRenderingPass_.Render(commandContext_, *camera, modelSorter_);
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, *sunLight);

        commandContext_.TransitionResource(lightingRenderingPass_.GetResult(), D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandContext_.TransitionResource(geometryRenderingPass_.GetDepth(), D3D12_RESOURCE_STATE_DEPTH_READ);
        commandContext_.SetViewportAndScissorRect(0, 0, lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
        commandContext_.SetRenderTarget(lightingRenderingPass_.GetResult().GetRTV(), geometryRenderingPass_.GetDepth().GetDSV());
        skybox_.SetWorldMatrix(Matrix4x4::MakeAffineTransform({ 1.0f, 1.0f, 1.0f }, Quaternion::identity, camera->GetPosition()));
        skybox_.Render(commandContext_, *camera);

        commandContext_.SetRenderTarget(lightingRenderingPass_.GetResult().GetRTV());
        commandContext_.SetViewportAndScissorRect(0, 0, lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
        lineDrawer_.Render(commandContext_, *camera);
    }

    bloom_.Render(commandContext_);
    fxaa_.Render(commandContext_);

    commandContext_.TransitionResource(finalImageBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(finalImageBuffer_.GetRTV());
    commandContext_.SetViewportAndScissorRect(0, 0, finalImageBuffer_.GetWidth(), finalImageBuffer_.GetHeight());

    postEffect_.Render(commandContext_, fxaa_.GetResult());

    // スワップチェーンに描画
    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext_.ClearColor(swapChainBuffer);
    commandContext_.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

#ifdef ENABLE_IMGUI

    commandContext_.TransitionResource(finalImageBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.FlushResourceBarriers();
    ImGui::Begin("Game", 0, ImGuiWindowFlags_NoScrollbar);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImTextureID image = reinterpret_cast<ImTextureID>(finalImageBuffer_.GetSRV().GetGPU().ptr);
    ImVec2 imageSize = CalcAspectFitSize(windowSize, { (float)finalImageBuffer_.GetWidth(), (float)finalImageBuffer_.GetHeight() });
    ImVec2 imageOffset = { (windowSize.x - imageSize.x) * 0.5f, (windowSize.y - imageSize.y) * 0.5f};
    ImGui::SetCursorPos(imageOffset);
    ImGui::Image(image, imageSize);
    ImGui::End();

    ImGui::Begin("Profile");
    auto io = ImGui::GetIO();
    ImGui::Text("Framerate : %f", io.Framerate);
    ImGui::Text("FrameCount : %d", frameCount_);
    postEffect_.DrawImGui("PostEffect");

    ImGui::End();

#endif // ENABLE_IMGUI

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext_);

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);

    // コマンドリスト完成(クローズ)
    commandContext_.Close();

    // バックバッファをフリップ
    swapChain_.Present();
    frameCount_++;
    // シグナルを発行し待つ
    auto& commandQueue = graphics_->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    commandQueue.WaitForIdle();

    commandContext_.Finish(false);

    graphics_->GetReleasedObjectTracker().FrameIncrementForRelease();

    timer_.KeepFrameRate(60);

    imguiManager->NewFrame();
    ShowDockingSpace();
}

void RenderManager::ShowDockingSpace() {
#ifdef ENABLE_IMGUI
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // ウィンドウフラグの設定
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // メインドッキングスペースの開始
    ImGui::Begin("DockSpace", nullptr, window_flags);

    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // ドッキングスペースの設定
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    // メニューバーの例
    //if (ImGui::BeginMenuBar())
    //{
    //    if (ImGui::BeginMenu("File"))
    //    {
    //        ImGui::MenuItem("Exit", NULL, false, false); // 無効なメニューアイテム
    //        ImGui::EndMenu();
    //    }
    //    ImGui::EndMenuBar();
    //}

    ImGui::End();
#endif // ENABLE_IMGUI
}
