#include "PlayerState.h"

void PlayerStateManager::Update() {
    if (standbyState_) {
        activeState_ = std::move(standbyState_);
        activeState_->Initialize();
    }

    if (activeState_) {
        activeState_->Update();
    }
}
