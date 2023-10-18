#pragma once

#include "MathUtils.h"

class Transform {
public:
    void UpdateMatrix() {
        worldMatrix = Matrix4x4::MakeAffineTransform(scale, rotate, translate);
        if (parent_) {
            worldMatrix *= parent_->worldMatrix;
        }
    }

    void SetParent(const Transform* parent) {
        parent_ = parent;
        if (parent_) {
            Matrix4x4 localMatrix = worldMatrix * parent_->worldMatrix.Inverse();
            scale = localMatrix.GetScale();
            rotate = localMatrix.GetRotate();
            translate = localMatrix.GetTranslate();
        }
    }

    Vector3 scale = Vector3::one;
    Quaternion rotate;
    Vector3 translate;
    Matrix4x4 worldMatrix;

private:
    const Transform* parent_ = nullptr;
};