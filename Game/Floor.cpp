#include "Floor.h"

#include "Graphics/ResourceManager.h"

void Floor::Initialize(const Vector3& basePosition, float radian, float movement, uint32_t moveParamCycle) {
    transform_ = std::make_shared<Transform>();

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Floor"));
    model_->SetIsActive(true);
    model_->SetUseOutline(false);

    basePosition_ = basePosition;
    transform_->translate = basePosition;
    transform_->rotate = Quaternion::MakeForYAxis(radian);
    transform_->scale = Vector3::one;
    
    movement_ = movement;
    moveParamCycle_ = moveParamCycle;

    moveParam_ = 0.0f;
}

void Floor::Update() {
    if (moveParamCycle_ > 0) {
        const float delta = Math::TwoPi / moveParamCycle_;
        moveParam_ += delta;
        moveParam_ = std::fmod(moveParam_, Math::TwoPi);
        Vector3 move = transform_->rotate * Vector3::forward * movement_ * std::sin(moveParam_);

        transform_->translate = basePosition_ + move;
    }

    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}
