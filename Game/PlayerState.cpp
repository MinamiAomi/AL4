#include "PlayerState.h"

#include "Player.h"
#include "Input/Input.h"

void PlayerStateManager::Update() {
    if (standbyState_) {
        activeState_ = std::move(standbyState_);
        activeState_->Initialize();
    }

    if (activeState_) {
        activeState_->Update();
    }
}

void PlayerStateManager::OnCollision(const CollisionInfo& collisionInfo) {
    if (activeState_) {
        activeState_->OnCollision(collisionInfo);
    }
}

void PlayerStateRoot::Initialize() {
    ySpeed_ = 0.0f;
}

void PlayerStateRoot::Update() {
    const auto& constantData = manager_.player.GetConstantData();
    auto& transform = manager_.player.transform;
    auto& camera = manager_.player.GetCamera()->GetCamera();
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
            move = camera->GetRotate() * move;
            move.y = 0.0f;
            move = move.Normalized() * constantData.moveSpeed;

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


    ySpeed_ -= constantData.gravity;

    auto& xinputState = input->GetXInputState();
    if (canJump_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        canJump_ = false;
        ySpeed_ += constantData.jumpPower;
    }

    ySpeed_ = std::max(ySpeed_, -constantData.maxFallSpeed);
    transform.translate.y += ySpeed_;

    if (input->IsKeyTrigger(DIK_TAB)) {
        manager_.ChangeState<PlayerStateAttack>();
    }
}

void PlayerStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
    if (collisionInfo.collider->GetName() == "Floor") {
        // 衝突位置の法線
        float dot = Dot(collisionInfo.normal, Vector3::up);
        // 地面と見なす角度
        const float kGroundGradientAngle = 45.0f * Math::ToRadian;
        if (std::abs(std::acos(dot)) < kGroundGradientAngle) {
            canJump_ = true;
            ySpeed_ = 0.0f;
        }
    }
}

void PlayerStateAttack::Initialize() {
}

void PlayerStateAttack::Update() {
}

void PlayerStateAttack::OnCollision(const CollisionInfo& collisionInfo) {
    collisionInfo;
}
