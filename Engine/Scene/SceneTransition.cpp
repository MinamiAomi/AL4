#include "SceneTransition.h"

#include "Graphics/RenderManager.h"

void SceneTransition::Update() {
    if (isPlaying_) {
        switch (mode_) {
        case SceneTransition::Mode::In:
            time_ += 1.0f / length_;
            if (time_ >= 1.0f) {
                time_ = 1.0f;
                isPlaying_ = false;
            }
            break;
        case SceneTransition::Mode::Out:
            time_ -= 1.0f / length_;
            if (time_ <= 0.0f) {
                time_ = 0.0f;
                isPlaying_ = false;
            }
            break;
        }
        RenderManager::GetInstance()->GetTransition().SetTime(time_);
    }
}

void SceneTransition::Start(Mode mode) {
    isPlaying_ = true;
    mode_ = mode;
    switch (mode) {
    case SceneTransition::Mode::In:
        time_ = 0.0f;
        break;
    case SceneTransition::Mode::Out:
        time_ = 1.0f;
        break;
    }
}
