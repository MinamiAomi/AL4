#pragma once
#include "Collision/GameObject.h"

#include <memory>
#include <optional>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"
#include "FollowCamera.h"
#include "Weapon.h"
#include "PlayerState.h"

class Player :
    public GameObject {
public:

    struct ConstantData {
        Vector3 colliderOffset;
        float moveSpeed;
        float gravity;
        float jumpPower;
        float maxFallSpeed;
        uint32_t dushTime;
        float dushSpeed;
    };
    
    void Initialize();
    void Update();

    void SetCamera(const std::shared_ptr<FollowCamera>& camera) { camera_ = camera; }
    const std::shared_ptr<FollowCamera>& GetCamera() const { return camera_; }
    const std::shared_ptr<Weapon>& GetWeapon() { return weapon_; }

    const ConstantData& GetConstantData() const { return constantData_; }

private:
    void Restart();
    void UpdateTransform();

    void RegisterGlobalVariables();
    void ApplyGlobalVariables();

    void OnCollision(const CollisionInfo& collisionInfo);


    std::unique_ptr<ToonModelInstance> model_;
    std::unique_ptr<BoxCollider> collider_;

    std::shared_ptr<FollowCamera> camera_;
    std::shared_ptr<Weapon> weapon_;

    std::unique_ptr<PlayerStateManager> state_;

    // 定数データ
    // ほかの状態でも使用される
    ConstantData constantData_;

    bool requestRestart_;
};