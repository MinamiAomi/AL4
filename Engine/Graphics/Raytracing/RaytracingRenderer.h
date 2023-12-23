#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../../Math/Camera.h"

#include "StateObject.h"
#include "TLAS.h"
#include "ShaderTable.h"

class CommandContext;
class DirectionalLight;

class RaytracingRenderer {
public:
    void Create(uint32_t width, uint32_t height);

    void Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight);

    ColorBuffer& GetResult() { return resultBuffer_; }

private:
    void CreateRootSignature();
    void CreateStateObject();
    void CreateShaderTables();
    void BuildScene(CommandContext& commandContext);

    StateObject stateObject_;
    RootSignature globalRootSignature_;
    TLAS tlas_;
    ShaderTable rayGenerationShaderTable_;
    ShaderTable hitGroupShaderTable_;
    ShaderTable missShaderTable_;

    ColorBuffer resultBuffer_;
};