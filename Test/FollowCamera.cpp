#include "FollowCamera.h"

#include "Graphics/RenderManager.h"
#include "Input/Input.h"

void FollowCamera::Initialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);
}

void FollowCamera::Update() {
    
    auto input = Input::GetInstance();



    camera_->SetPosition(transform.translate);
    camera_->SetRotate(transform.rotate);
    camera_->UpdateMatrices();
}