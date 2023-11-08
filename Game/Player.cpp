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

    transform.translate = Vector3::zero;
    transform.scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Player"));
    model_->SetIsActive(true);
    //model_->SetUseOutline(false);
    model_->SetOutlineWidth(0.03f);
    model_->SetOutlineColor({ 0.0f,0.0f,0.0f });
    ySpeed_ = 0.0f;

    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Player");
    collider_->SetCenter(transform.translate + colliderOffset_);
    collider_->SetSize({ 1.0f, 2.0f, 1.0f });
    collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
    collider_->SetCollisionAttribute(CollisionAttribute::Player);
    collider_->SetCollisionMask(~CollisionAttribute::Player);
    //collider_->SetIsActive(false);

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
        MoveUpdate();
        JumpUpdate();
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
    ySpeed_ = 0.0f;
    requestRestart_ = false;
    if (camera_) {
        camera_->Restart();
    }
}

void Player::MoveUpdate() {
    auto input = Input::GetInstance();

    Vector3 move{};
    // Gamepad入力
    {
        auto& xinputState = input->GetXInputState();
        const float margin = 0.8f;
        const float shortMaxReci = 1.0f / float(SHRT_MAX);
        move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
        move *= shortMaxReci;
        if (move.Length() < margin) {
            move = Vector3::zero;
        }
    }

    // 移動処理
    {
        if (move != Vector3::zero) {
            move = move.Normalized();
            // 地面に水平なカメラの回転
            move = camera_->GetCamera()->GetRotate() * move;
            move.y = 0.0f;
            move = move.Normalized() * moveSpeed_;

            // 親がいる場合親の空間にする
            const Transform* parent = transform.GetParent();
            if (parent) {
                move = parent->worldMatrix.Inverse().ApplyRotation(move);
            }

            // 移動
            transform.translate += move;
            // 回転
            //transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));
                
            move = transform.rotate.Conjugate() * move;
            Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
            transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;
        }
    }
}

void Player::JumpUpdate() {
    auto input = Input::GetInstance();

    ySpeed_ -= gravity_;

    auto& xinputState = input->GetXInputState();
    if (canJump_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        canJump_ = false;
        ySpeed_ += jumpPower_;
    }

    ySpeed_ = std::max(ySpeed_, -maxFallSpeed_);
    transform.translate.y += ySpeed_;
}

void Player::UpdateTransform() {
    transform.UpdateMatrix();
    Vector3 scale, translate;
    Quaternion rotate;
    transform.worldMatrix.GetAffineValue(scale, rotate, translate);
    collider_->SetCenter(translate + colliderOffset_);
    collider_->SetOrientation(rotate);
    model_->SetWorldMatrix(transform.worldMatrix);
    weapon_->UpdateTransform();
}

void Player::OnCollision(const CollisionInfo& collisionInfo) {
    if (collisionInfo.collider->GetName() == "Floor") {
        // ワールド空間の押し出しベクトル
        Vector3 pushVector = collisionInfo.normal * collisionInfo.depth;
        auto parent = transform.GetParent();
        if (parent) {
            pushVector = parent->rotate.Inverse() * pushVector;
        }
        transform.translate += pushVector;

        // 衝突位置の法線
        float dot = Dot(collisionInfo.normal, Vector3::up);
        // 地面と見なす角度
        const float kGroundGradientAngle = 45.0f * Math::ToRadian;
        if (std::abs(std::acos(dot)) < kGroundGradientAngle) {
            canJump_ = true;
            ySpeed_ = 0.0f;
        }
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
