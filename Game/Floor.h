#pragma once

#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"

class Floor {
public:
    virtual ~Floor() {}

    virtual void Initialize();
    virtual void Update();

    const std::shared_ptr<Transform>& GetTransform() const { return transform_; }

protected:
    std::shared_ptr<Transform> transform_;
    std::unique_ptr<ToonModelInstance> model_;
};