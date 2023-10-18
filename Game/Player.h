#pragma once

#include <memory>

#include "Math/Transform.h"
#include "Math/Camera.h"
#include "Graphics/ToonModel.h"

class Player {
public:
    void Initialize();
    void Update();

    const std::shared_ptr<Transform>& GetTransform() const { return transform_; }

    void SetCamera(const std::shared_ptr<const Camera>& camera) { camera_ = camera; }

private:
    void MoveUpdate();
    void JumpUpdate();

    std::shared_ptr<Transform> transform_;
    std::unique_ptr<ToonModelInstance> model_;

    std::shared_ptr<const Camera> camera_;

    float prevYTranslate_;

    float moveSpeed_ = 0.3f;
    float jumpForce_ = 0.3f;
    float fallForce_ = -0.03f;

    bool canJump_;
};