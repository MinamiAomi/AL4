#pragma once

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"

class CommandContext;
class ColorBuffer;

class Denoiser {
public:

    void Initialize();
    void Render(CommandContext& commandContext, ColorBuffer& intermadiateBuffer, ColorBuffer& accumulationBuffer, ColorBuffer& denoisedBuffer, uint32_t sampleCount);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;

};
