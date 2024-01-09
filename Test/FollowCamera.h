#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"

class FollowCamera :
    public GameObject {
public:
    void Initialize();
    void Update();
    
    void SetTarget(const std::shared_ptr<GameObject>& target) { target_ = target; }
    
    const std::shared_ptr<Camera>& GetCamera() const { return camera_; }

private:
    std::shared_ptr<Camera> camera_;
    Vector3 eulerAngle_;

    std::weak_ptr<GameObject> target_;
};