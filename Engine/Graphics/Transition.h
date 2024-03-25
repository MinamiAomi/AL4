#pragma once
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Math/MathUtils.h"

class CommandContext;
class ColorBuffer;

class Transition {
public:
    void Initialize();
    void Dispatch(CommandContext& commandContext, ColorBuffer& texture);

    void SetTime(float t) { time_ = t; }
    void SetFadeColor(const Vector3& color) { fadeColor_ = color; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    Vector3 fadeColor_;
    float time_ = 0.0f;
};