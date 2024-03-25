#include "FollowCamera.h"

#include "Graphics/RenderManager.h"
#include "Input/Input.h"

void FollowCamera::Initialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

    eulerAngle_ = Vector3(10.0f, 0.0f, 0.0f) * Math::ToRadian;

}

void FollowCamera::Update() {
    auto input = Input::GetInstance();
    XINPUT_STATE xinputState = input->GetXInputState();

    if (std::abs(xinputState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        const float rotateSpeed = 3.0f * Math::ToRadian;
        eulerAngle_.y += float(xinputState.Gamepad.sThumbRX) / float(SHRT_MAX) * rotateSpeed;
    }
    if (std::abs(xinputState.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        const float rotateSpeed = 2.0f * Math::ToRadian;
        eulerAngle_.x += -float(xinputState.Gamepad.sThumbRY) / float(SHRT_MAX) * rotateSpeed;
    }

    eulerAngle_.x = std::clamp(eulerAngle_.x, -10.0f * Math::ToRadian, 80.0f * Math::ToRadian);
    eulerAngle_.x = std::fmod(eulerAngle_.x, Math::TwoPi);
    eulerAngle_.y = std::fmod(eulerAngle_.y, Math::TwoPi);
    transform.rotate = Quaternion::MakeFromEulerAngle(eulerAngle_);

    auto target = target_.lock();
    if (target) {
        Vector3 offset = { 0.0f, 3.0f, -15.0f };
        offset = transform.rotate * offset;

        transform.translate = target->transform.translate + offset;
    }


    camera_->SetPosition(transform.translate);
    camera_->SetRotate(transform.rotate);
    camera_->UpdateMatrices();
}