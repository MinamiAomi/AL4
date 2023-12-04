#pragma once

#include "../Core/ColorBuffer.h"
#include "../../Math/Camera.h"

class CommandContext;

class RaytracingRenderer {
public:
    void Create();

    void Render(CommandContext& commandContext, const Camera& camera);


private:
    void CreateStateObject();


    Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject_;

    ColorBuffer resultBuffer_;

};