#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"
#include "FollowCamera.h"

class Player :
    public GameObject {
public:
    void Initialize();
    void Update();

    void SetCamera(const std::shared_ptr<FollowCamera>& camera) { camera_ = camera; }
    
private:
    void Restart();
    void MoveUpdate();
    void JumpUpdate();
    void UpdateTransform();

    void OnCollision(const CollisionInfo& collisionInfo);

    std::unique_ptr<ToonModelInstance> model_;
    std::unique_ptr<BoxCollider> collider_;

    std::shared_ptr<FollowCamera> camera_;

    //float prevYTranslate_ = 0.0f;

    Vector3 colliderOffset_ = { 0.0f, 1.0f, 0.0f };
    float ySpeed_ = 0.0f;
    float moveSpeed_ = 0.3f;
    float gravity_ = 0.02f;;
    float jumpPower_ = 0.3f;
    float maxFallSpeed_ = 0.5f;

    bool canJump_;
    bool requestRestart_;
};