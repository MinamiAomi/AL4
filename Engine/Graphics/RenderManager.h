#pragma once

#include "Math/Camera.h"
#include "Core/Graphics.h"
#include "Core/SwapChain.h"
#include "Core/CommandContext.h"
#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Bloom.h"
#include "ToonRenderer.h"
#include "ParticleRenderer.h"
#include "SpriteRenderer.h"
#include "PostEffect.h"
#include "Timer.h"

class RenderManager {
public:

    static RenderManager* GetInstance();

    void Initialize();
    void Finalize();
    void Render();

    void SetCamera(const std::shared_ptr<Camera>& camera) { camera_ = camera; }

private:
    RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContext_;

    ColorBuffer mainColorBuffer_;
    DepthBuffer mainDepthBuffer_;

    ToonRenderer toonRenderer_;
    ParticleRenderer particleRenderer_;
    SpriteRenderer spriteRenderer_;
    Bloom bloom_;
    PostEffect postEffect_;
    
    Timer timer_;
    std::shared_ptr<const Camera> camera_;
};