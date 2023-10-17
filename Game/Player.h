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

    std::shared_ptr<Transform> transform_;
    std::unique_ptr<ToonModelInstance> model_;

    std::shared_ptr<const Camera> camera_;
};