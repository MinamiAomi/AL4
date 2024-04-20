#pragma once
#include <memory>

#include "Core/ColorBuffer.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Math/Camera.h"
#include "LightManager.h"

class CommandContext;
class GeometryRenderingPass;

class LightingRenderingPass {
public:
    struct RootIndex {
        enum Parameters {
            Scene,
            Albedo,
            MetallicRoughness,
            Normal,
            Depth,

            NumRootParameters
        };
    };

    void Initialize(uint32_t width, uint32_t height);
    void Render(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass, const Camera& camera, const DirectionalLight& light);

    ColorBuffer& GetResult() { return result_; }

private:
    ColorBuffer result_;
    RootSignature rootSignature_;
    PipelineState pipelineState_;
};