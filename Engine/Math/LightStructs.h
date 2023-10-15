#pragma once

#include "MathUtils.h"

struct DirectionalLight {
    Vector4 color;
    Vector3 direction;
    float intensity = 1.0f;
};