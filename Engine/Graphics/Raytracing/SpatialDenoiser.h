#pragma once

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../Core/ColorBuffer.h"
#include "../GeometryRenderingPass.h"

class CommandContext;

class SpatialDenoiser {
public:
    void Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format);
    void Dispatch(CommandContext& commandContext, ColorBuffer& sourceBuffer, GeometryRenderingPass& gBuffers);

    ColorBuffer& GetDenoisedBuffer() { return denoisedBuffer_; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ColorBuffer denoisedBuffer_;
};
