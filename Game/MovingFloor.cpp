#include "MovingFloor.h"

void MovingFloor::Initialize() {
    Floor::Initialize();
    moveParam_ = 0.0f;
}

void MovingFloor::Update() {

    const uint32_t moveParamCycle = 120;
    const float delta = Math::TwoPi / moveParamCycle;
    moveParam_ += delta;
    moveParam_ = std::fmod(moveParam_, Math::TwoPi);

    transform_->translate += std::cos(moveParam_) * (transform_->rotate * Vector3::forward);

    Floor::Update();
}
