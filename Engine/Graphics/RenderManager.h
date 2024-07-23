#pragma once

#include "Math/Camera.h"
#include "Core/Graphics.h"
#include "Core/SwapChain.h"
#include "Core/CommandContext.h"
#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Raytracing/RaytracingRenderer.h"
#include "RaymarchingRenderer.h"
#include "ModelRenderer.h"
#include "Bloom.h"
#include "FXAA.h"
#include "Graysale.h"
#include "ParticleRenderer.h"
#include "SpriteRenderer.h"
#include "PostEffect.h"
#include "Timer.h"
#include "LightManager.h"
#include "ComputeShader.h"
#include "Transition.h"
#include "ModelSorter.h"
#include "SkinningManager.h"
#include "GeometryRenderingPass.h"
#include "LightingRenderingPass.h"
#include "Skybox.h"
#include "LineDrawer.h"
#include "Raytracing/TestRTRenderer.h"

#ifdef _DEBUG
#define SHADER_DIRECTORY "../Engine/Graphics/Shader"
#else
#define SHADER_DIRECTORY "Resources/Shader"
#endif // _DEBUG

class RenderManager {
public:

    static RenderManager* GetInstance();

    void Initialize();
    void Finalize();
    void Render();

    void SetCamera(const std::shared_ptr<Camera>& camera) { camera_ = camera; }
    void SetSunLight(const std::shared_ptr<DirectionalLight>& light) { sunLight_ = light; }

    //LightManager& GetLightManager() { return lightManager_; }
    SwapChain& GetSwapChain() { return swapChain_; }
    ColorBuffer& GetFinalImageBuffer() { return finalImageBuffer_; }
    Transition& GetTransition() { return transition_; }
    SkinningManager& GetSkinningManager() { return skinningManager_; }
    LineDrawer& GetLineDrawer() { return lineDrawer_; }
    Skybox& GetSkybox() { return skybox_; }
    LightingRenderingPass& GetLightingRenderingPass() { return lightingRenderingPass_; }
    TestRTRenderer& GetTestRTRenderer() { return testRTRenderer_; }
    PostEffect& GetPostEffect() { return postEffect_; }

private:
    RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    void ShowDockingSpace();

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContext_;

    ColorBuffer finalImageBuffer_;

    SpriteRenderer spriteRenderer_;

    ModelSorter modelSorter_;
    SkinningManager skinningManager_;
    GeometryRenderingPass geometryRenderingPass_;
    LightingRenderingPass lightingRenderingPass_;
    Skybox skybox_;
    LineDrawer lineDrawer_;

    
    TestRTRenderer testRTRenderer_;
    //RaytracingRenderer raytracingRenderer_;
    //RaymarchingRenderer raymarchingRenderer_;
    //ModelRenderer modelRenderer;
    Transition transition_;
    //Bloom bloom_;
    FXAA fxaa_;
    PostEffect postEffect_;
    ComputeShaderTester computeShaderTester_;
    //LightManager lightManager_;
    
    Timer timer_;
    std::weak_ptr<const Camera> camera_;
    std::weak_ptr<const DirectionalLight> sunLight_;

    UINT64 frameCount_;
    bool raymarching_ = false;
};