#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../../Math/Camera.h"

#include "StateObject.h"
#include "TLAS.h"

class CommandContext;

class RaytracingRenderer {
public:
    void Create(uint32_t width, uint32_t height);

    void Render(CommandContext& commandContext, const Camera& camera);


private:
    void CreateRootSignature();
    void CreateStateObject();

    StateObject stateObject_;
    RootSignature globalRootSignature_;
    TLAS tlas_;

    ColorBuffer resultBuffer_;

};