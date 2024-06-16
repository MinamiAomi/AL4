#pragma once

#include <memory>
#include "Math/Transform.h"
#include "Graphics/Model.h"
#include "Graphics/Skeleton.h"

class Player {
public:
    void Initialize();
    void Update();

private:
    Transform transform_;
    ModelInstance model_;
    ModelInstance hammer_;
    std::shared_ptr<Skeleton> skeleton_;
    std::shared_ptr<Animation> walk_;
    float animationTime_;

};