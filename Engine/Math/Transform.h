#pragma once

#include "MathUtils.h"

class Transform {
public:
    void UpdateMatrix() {
        worldMatrix = Matrix4x4::MakeAffineTransform(scale, rotate, translate);
        if (parent) {
            worldMatrix *= parent->worldMatrix;
        }
    }

    Vector3 scale = Vector3::one;
    Quaternion rotate;
    Vector3 translate;
    Matrix4x4 worldMatrix;
    const Transform* parent = nullptr;
};