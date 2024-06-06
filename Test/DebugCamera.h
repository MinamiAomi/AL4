#pragma once

#include <memory>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"

class DebugCamera {
public:
    void Initialize();
    void Update();

    const std::shared_ptr<Camera>& GetCamera() const { return camera_; }

private:
    Transform transform;
    std::shared_ptr<Camera> camera_;
    Vector3 eulerAngle_;
};