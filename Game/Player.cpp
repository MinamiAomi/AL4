#include "Player.h"

#include <cassert>

#include "Input/Input.h"
#include "Graphics/ResourceManager.h"
#include "CollisionAttribute.h"
#include "Collision/CollisionManager.h"

#include "Graphics/ImGuiManager.h"

#include "Weapon.h"

static Vector3 n;

void Player::Initialize() {
    SetName("Player");

    constantData_.colliderOffset = { 0.0f, 1.0f, 0.0f };
    constantData_.moveSpeed = 0.3f;
    constantData_.gravity = 0.02f;;
    constantData_.jumpPower = 0.3f;
    constantData_.maxFallSpeed = 0.5f;

    transform.translate = Vector3::zero;
    transform.scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Player"));
    model_->SetIsActive(true);
    //model_->SetUseOutline(false);
    model_->SetOutlineWidth(0.03f);
    model_->SetOutlineColor({ 0.0f,0.0f,0.0f });

    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Player");
    collider_->SetCenter(transform.translate + constantData_.colliderOffset);
    collider_->SetSize({ 1.0f, 2.0f, 1.0f });
    collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
    collider_->SetCollisionAttribute(CollisionAttribute::Player);
    collider_->SetCollisionMask(~CollisionAttribute::Player);
    //collider_->SetIsActive(false);


    state_ = std::make_unique<PlayerStateManager>(*this);
    state_->ChangeState<PlayerStateRoot>();

    weapon_ = std::make_shared<Weapon>();
    weapon_->Initialize();
    //weapon_->SetIsShowing(false);
    weapon_->transform.SetParent(&transform);
}

void Player::Update() {
    if (requestRestart_) {
        Restart();
    }
    else {
        state_->Update();
    }

    if (transform.worldMatrix.GetTranslate().y < -10.0f) {
        requestRestart_ = true;
    }

    UpdateTransform();
    weapon_->Update();

    ImGui::Begin("Player");
    ImGui::DragFloat3("normal", &n.x);
    ImGui::End();
}

void Player::Restart() {
    transform.SetParent(nullptr);
    transform.translate = Vector3::zero;
    transform.rotate = Quaternion::identity;
    requestRestart_ = false;
    if (camera_) {
        camera_->Restart();
    }
    state_->ChangeState<PlayerStateRoot>();
}

void Player::UpdateTransform() {
    transform.UpdateMatrix();
    Vector3 scale, translate;
    Quaternion rotate;
    transform.worldMatrix.GetAffineValue(scale, rotate, translate);
    collider_->SetCenter(translate + constantData_.colliderOffset);
    collider_->SetOrientation(rotate);
    model_->SetWorldMatrix(transform.worldMatrix);
    weapon_->UpdateTransform();
}

void Player::OnCollision(const CollisionInfo& collisionInfo) {
    state_->OnCollision(collisionInfo);

    if (collisionInfo.collider->GetName() == "Floor") {
        // ワールド空間の押し出しベクトル
        Vector3 pushVector = collisionInfo.normal * collisionInfo.depth;
        auto parent = transform.GetParent();
        if (parent) {
            pushVector = parent->rotate.Inverse() * pushVector;
        }
        transform.translate += pushVector;

        UpdateTransform();

        const GameObject* nextParent = collisionInfo.collider->GetGameObject();
        if (nextParent) {
            transform.SetParent(&nextParent->transform);
        }
    }
    else if (collisionInfo.collider->GetName() == "Goal") {
        requestRestart_ = true;
    }
    else if (collisionInfo.collider->GetName() == "Enemy") {
        requestRestart_ = true;
    }
}
