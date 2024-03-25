#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"

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
    float clearColor[4] = { 0.1f, 0.4f, 0.6f, 0.0f };
    mainColorBuffer_.SetClearColor(clearColor);
    mainColorBuffer_.Create(L"MainColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    preSwapChainBuffer_.Create(L"PreSwapChainBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    mainDepthBuffer_.Create(L"MainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    particleRenderer_.Initialize(mainColorBuffer_, mainDepthBuffer_);
    postEffect_.Initialize(preSwapChainBuffer_);
    spriteRenderer_.Initialize(preSwapChainBuffer_);

    geometryRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    lightingRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    fxaa_.Initialize(&lightingRenderingPass_.GetResult());

//    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    transition_.Initialize();
    raytracingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());
    particleRenderer_.Initialize(mainColorBuffer_, mainDepthBuffer_);

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

    if (camera && sunLight) {
        // 影、スペキュラ
    //    raytracingRenderer_.Render(commandContext_, *camera, *sunLight);
        geometryRenderingPass_.Render(commandContext_, *camera);
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, *sunLight);
    }

    fxaa_.Render(commandContext_);

    commandContext_.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandContext_.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
    commandContext_.ClearColor(mainColorBuffer_);
    commandContext_.ClearDepth(mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());


    if (camera && sunLight) {
        // モデル描画
        //modelRenderer.Render(commandContext_, *camera, *sunLight);
        particleRenderer_.Render(commandContext_, *camera);
        //raymarchingRenderer_.Render(commandContext_, *camera);
    }
    // レイトレの結果を加算合成
    //postEffect_.RenderAddTexture(commandContext_, raytracingRenderer_.GetSpecular());
   // postEffect_.RenderMultiplyTexture(commandContext_, raytracingRenderer_.GetShadow());


    commandContext_.TransitionResource(preSwapChainBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(preSwapChainBuffer_.GetRTV());
    commandContext_.ClearColor(preSwapChainBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, preSwapChainBuffer_.GetWidth(), preSwapChainBuffer_.GetHeight());

    postEffect_.Render(commandContext_, mainColorBuffer_);
    spriteRenderer_.Render(commandContext_, 0.0f, 0.0f, float(preSwapChainBuffer_.GetWidth()), float(preSwapChainBuffer_.GetHeight()));

    transition_.Dispatch(commandContext_, preSwapChainBuffer_);

    
    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.CopyBuffer(swapChainBuffer, fxaa_.GetResult());

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.FlushResourceBarriers();
    commandContext_.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext_.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());


#ifdef ENABLE_IMGUI
    ImGui::Begin("Profile");
    auto io = ImGui::GetIO();
    ImGui::Text("Framerate : %f", io.Framerate);
    ImGui::Text("FrameCount : %d", frameCount_);

    //transition_.SetTime(t);
    auto ImagePreview = [](const char* name, const DescriptorHandle& srv, const ImVec2& size) {
        if (ImGui::TreeNode(name)) {
            ImTextureID image = reinterpret_cast<ImTextureID>(srv.GetGPU().ptr);
            ImGui::Image(image, size);
            ImGui::TreePop();
        }
        };
    commandContext_.TransitionResource(geometryRenderingPass_.GetAlbedo(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.TransitionResource(geometryRenderingPass_.GetMetallicRoughness(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.TransitionResource(geometryRenderingPass_.GetNormal(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.TransitionResource(geometryRenderingPass_.GetDepth(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.FlushResourceBarriers();
    ImagePreview("Albedo", geometryRenderingPass_.GetAlbedo().GetSRV(), {360.0f, 180.0f});
    ImagePreview("MetallicRoughness", geometryRenderingPass_.GetMetallicRoughness().GetSRV(), { 360.0f, 180.0f });
    ImagePreview("Normal", geometryRenderingPass_.GetNormal().GetSRV(), { 360.0f, 180.0f });
    ImagePreview("Depth", geometryRenderingPass_.GetDepth().GetSRV(), { 360.0f, 180.0f });

    //ImagePreview("MainColorBuffer", mainColorBuffer_.GetSRV(), { 320.0f, 180.0f });
    //ImagePreview("MainDepthBuffer", mainDepthBuffer_.GetSRV(), { 320.0f, 180.0f });
    //ImagePreview("SpecularBuffer", raytracingRenderer_.GetSpecular().GetSRV(), { 320.0f, 180.0f });
    //ImagePreview("ShadowBuffer", raytracingRenderer_.GetShadow().GetSRV(), { 320.0f, 180.0f });
    //ImagePreview("Raymatching", raymarchingRenderer_.GetResult().GetSRV(), { 320.0f, 180.0f });
    //ImagePreview("Noise", computeShaderTester_.GetTexture().GetSRV(), { 320.0f, 320.0f });

    //ImGui::Checkbox("Raymarching", &raymarching_);
    ImGui::End();
#endif // ENABLE_IMGUI

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext_);

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
    // commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

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
}