#pragma once
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"

class CommandContext;
class ColorBuffer;

class FXAA {
public:
    void Initialize();
    void Render(CommandContext& commandContext, ColorBuffer& texture);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
};