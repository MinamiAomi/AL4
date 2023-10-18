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
    //model_->SetUseOutline(false);
   // model_->SetOutlineColor({ 1.0f,0.0f,0.0f });
}

void Player::Update() {

    MoveUpdate();

    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}

void Player::MoveUpdate() {
    auto input = Input::GetInstance();

    const float speed = 0.3f;
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
            move = move.Normalized() * speed;
            // 地面に水平なカメラの回転
            //Quaternion cameraRotate = Quaternion::MakeLookRotation(Vector3::Scale(camera_->GetForward(), { 1.0f,0.0f,1.0f }));
            move = camera_->GetRotate() * move;
            // 移動
            transform_->translate += move;
            // 回転
            transform_->rotate = Quaternion::Slerp(0.1f, transform_->rotate, Quaternion::MakeLookRotation(move));
        }
    }

}
