#pragma once

#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"

class Floor {
public:
    ~Floor() {}

    void Initialize(const Vector3& basePosition, float radian, float movement, uint32_t moveParamCycle);
    void Update();

    const std::shared_ptr<Transform>& GetTransform() const { return transform_; }

    void SetMovement(float movement) { movement_ = movement; }
    void SetMoveParamCycle(uint32_t moveParamCycle) { moveParamCycle_ = moveParamCycle; }

protected:
    std::shared_ptr<Transform> transform_;
    std::unique_ptr<ToonModelInstance> model_;

    Vector3 basePosition_;
    float movement_ = 0.0f;
    float moveParam_ = 0;
    uint32_t moveParamCycle_ = 120;
};