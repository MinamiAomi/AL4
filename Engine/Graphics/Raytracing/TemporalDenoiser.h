#pragma once
/// ただバッファに加算していくだけ

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../Core/ColorBuffer.h"

class CommandContext;

class TemporalDenoiser {
public:
    void Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format);
    void Dispatch(CommandContext& commandContext, ColorBuffer& intermediateBuffer, ColorBuffer& denoisedBuffer);
    void Reset(CommandContext& commandContext);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ColorBuffer accumulationBuffer_;
    uint32_t sampleCount_;
};