#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../../Math/Camera.h"

#include "StateObject.h"
#include "TLAS.h"

class CommandContext;

class RaytracingRenderer {
public:
    void Create();

    void Render(CommandContext& commandContext, const Camera& camera);


private:
    void CreateRootSignature();
    void CreateStateObject();

    StateObject stateObject_;
    RootSignature globalRootSignature_;
    RootSignature primaryRayHitGroupLocalRootSignature_;
    TLAS tlas_;

    ColorBuffer resultBuffer_;

};