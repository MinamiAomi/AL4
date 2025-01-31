#pragma once

#include "Core/ColorBuffer.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"

namespace LIEngine {

    class CommandContext;

    class ComputeShaderTester {
    public:
        void Initialize(uint32_t width, uint32_t height);
        void Dispatch(CommandContext& commandContext);
        const ColorBuffer& GetTexture() const { return texture_; }

    private:
        RootSignature rootSignature_;
        PipelineState pipelineState_;
        ColorBuffer texture_;
    };

}