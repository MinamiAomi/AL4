#pragma once
#include <memory>

#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Math/Camera.h"

class CommandContext;

class DeferredRenderer {
public:
    void Initialize();
    void Render(CommandContext& commandContext, std::shared_ptr<Camera> camera);

private:
    ColorBuffer albedo_;
    ColorBuffer metallicRoughness_;
    ColorBuffer normal_;
    DepthBuffer depth_;
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    

};