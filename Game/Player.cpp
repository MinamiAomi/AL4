#include "Player.h"

#include <cassert>

#include "Input/Input.h"

void Player::Initialize() {
    transform_ = std::make_shared<Transform>();
    transform_->translate = Vector3::zero;
    transform_->scale = Vector3::one;
}

void Player::Update() {

    transform_->UpdateMatrix();
}

void Player::MoveUpdate() {
    auto input = Input::GetInstance();

    const float speed = 0.3f;
    Vector3 move{};

    // Gamepad入力
    {
        auto xinputState = input->GetXInputState();
        const float margin = 0.0f;
        move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
        if (move.LengthSquare() < margin * margin) {
            move = Vector3::zero;
        }
    }

    // 移動処理
    {
        if (move != Vector3::zero) {
            move = move.Normalized() * speed;
            auto camera camera_->GetForward
        }
    }

}
