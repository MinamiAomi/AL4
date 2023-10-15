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
    graphics_->Initialize();

    ShaderManager::GetInstance()->Initialize();

    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    for (auto& commandContext : commandContexts_) {
        commandContext.Create();
        commandContext.Close();
    }

    DefaultTexture::Initialize();

    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    float clearColor[4] = { 0.1f, 0.4f, 0.6f, 0.0f };
    mainColorBuffer_.SetClearColor(clearColor);
    mainColorBuffer_.Create(L"MainColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    mainDepthBuffer_.Create(L"MainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    toonRenderer_.Initialize(mainColorBuffer_, mainDepthBuffer_);

    postEffect_.Initialize(swapChainBuffer);

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHWND(), swapChainBuffer.GetFormat());
    imguiManager->NewFrame();
}

void RenderManager::Finalize() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();

    DefaultTexture::Finalize();

    graphics_->Finalize();
}

void RenderManager::Render() {
    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];


    commandContext.Reset();
    
    commandContext.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandContext.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
    commandContext.ClearColor(mainColorBuffer_);
    commandContext.ClearDepth(mainDepthBuffer_);
    commandContext.SetViewportAndScissorRect(0, 0, mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    if (camera_) {
        toonRenderer_.Render(commandContext, *camera_);
    }

    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext.ClearColor(swapChainBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    
    postEffect_.Render(commandContext, mainColorBuffer_);
    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext);

    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandContext.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    commandQueue.WaitForGPU();
    commandQueue.Excute(commandContext);
    swapChain_.Present();
    commandQueue.Signal();

    imguiManager->NewFrame();
}