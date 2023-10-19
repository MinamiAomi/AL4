#include "Player.h"

#include <cassert>

#include "Input/Input.h"
#include "Graphics/ResourceManager.h"

void Player::Initialize() {
    transform_ = std::make_shared<Transform>();
    transform_->translate = Vector3::zero;
    transform_->scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Player"));
    model_->SetIsActive(true);
    model_->SetUseOutline(false);
    model_->SetOutlineWidth(0.03f);
    model_->SetOutlineColor({ 1.0f,0.0f,1.0f });
    prevYTranslate_ = 0.0f;
}

void Player::Update() {

    MoveUpdate();
    JumpUpdate();

    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}

void Player::MoveUpdate() {
    auto input = Input::GetInstance();

    Vector3 move{};
    // Gamepad入力
    {
        auto xinputState = input->GetXInputState();
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
            move = move.Normalized() * moveSpeed_;
            // 地面に水平なカメラの回転
            move = camera_->GetRotate() * move;
            // 移動
            transform_->translate += move;
            // 回転
            transform_->rotate = Quaternion::Slerp(0.1f, transform_->rotate, Quaternion::MakeLookRotation(move));
        }
    }
}

void Player::JumpUpdate() {
    auto input = Input::GetInstance();

    float force = fallForce_;

    auto xinputState = input->GetXInputState();
    if (canJump_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        force = jumpForce_;
        canJump_ = false;
    }

    auto yTmp = transform_->translate.y;
    transform_->translate.y += (transform_->translate.y - prevYTranslate_) + force;
    prevYTranslate_ = yTmp;

    if (transform_->translate.y <= 0.0f) {
        canJump_ = true;
        transform_->translate.y = 0.0f;
    }
}
