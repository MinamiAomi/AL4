#pragma once

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"

class FollowCamera {
public:
    void Initialize();
    void Update();
    void Restart();

    const std::shared_ptr<Transform>& GetTransform() { return transform_; }
    const std::shared_ptr<Camera>& GetCamera() { return camera_; }

    void SetTarget(const Transform* target) { target_ = target; Reset(); }

private:
    void Reset();
    Vector3 CalcOffset() const;

    void RegisterGlobalVariables();
    void ApplyGlobalVariables();

    std::shared_ptr<Transform> transform_;
    std::shared_ptr<Camera> camera_;

    const Transform* target_;
    Vector3 interTarget_;
    Quaternion destinationRotate_;

    float followDelay_;
};