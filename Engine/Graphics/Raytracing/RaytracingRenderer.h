#pragma once

#include "../Core/ColorBuffer.h"

class CommandContext;

class RaytracingRenderer {
public:
    void Create();

    void Render(CommandContext& commandContext);


private:
    ColorBuffer resultBuffer_;

};