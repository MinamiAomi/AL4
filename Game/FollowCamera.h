#pragma once

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"

class FollowCamera {
public:
    void Initialize();
    void Update();

    const std::shared_ptr<Transform>& GetTransform() { return transform_; }
    const std::shared_ptr<Camera>& GetCamera() { return camera_; }

    void SetTarget(const std::shared_ptr<const Transform>& target) { target_ = target; }

private:
    std::shared_ptr<Transform> transform_;
    std::shared_ptr<Camera> camera_;

    std::shared_ptr<const Transform> target_;
};