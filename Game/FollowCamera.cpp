#include "FollowCamera.h"

#include <cassert>

#include "Graphics/RenderManager.h"
#include "Input/Input.h"

void FollowCamera::Initialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    transform_ = std::make_shared<Transform>();
}

void FollowCamera::Update() {
    auto input = Input::GetInstance();

    XINPUT_STATE xinputState = input->GetXInputState();

    Quaternion diffRotate;
    // 左右振り向き
    if (std::abs(xinputState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        const float rotateSpeed = 3.0f * Math::ToRadian;
        transform_->rotate = Quaternion::MakeForYAxis(float(xinputState.Gamepad.sThumbRX) / float(SHRT_MAX) * rotateSpeed) * transform_->rotate;
    }

    if (target_) {
        Vector3 offset = { 0.0f, 3.0f, -10.0f };
        offset = transform_->rotate * offset;
        transform_->translate = target_->worldMatrix.GetTranslate() + offset;
    }

    camera_->SetPosition(transform_->translate);
    camera_->SetRotate(transform_->rotate);
    camera_->UpdateMatrices();
}
