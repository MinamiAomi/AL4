#pragma once
#include "Collision/GameObject.h"

#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"

class Enemy :
    public GameObject {
public:
    enum class Part {
        Body,
        Head,

        NumParts
    };

    void Initialize(const Vector3& basePosition);
    void Update();

private:
    struct PartData {
        ToonModelInstance model;
        Transform transform;
    };
    
    void UpdateAnimation();
    void OnCollision(const CollisionInfo& collisionInfo);
    
    std::unique_ptr<PartData> parts_[static_cast<size_t>(Part::NumParts)];
    std::unique_ptr<BoxCollider> collider_;

    float animationParameter_;
    bool isDead_;
};