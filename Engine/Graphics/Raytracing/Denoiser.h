#pragma once

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"

class CommandContext;

class Denoiser {
public:

    void Initialize();
    void Render(CommandContext& commandContext, ColorBuffer& target);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;

};
