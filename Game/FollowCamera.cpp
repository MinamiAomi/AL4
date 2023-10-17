#include "FollowCamera.h"

#include <cassert>

#include "Graphics/RenderManager.h"

void FollowCamera::Initialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    transform_ = std::make_shared<Transform>();
}

void FollowCamera::Update() {

    camera_->UpdateMatrices();
}
