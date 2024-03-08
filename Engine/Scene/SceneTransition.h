#pragma once

#include <cstdint>

class SceneTransition {
public: 
    enum class Mode {
        Ready,
        In,
        Wait,
        Out,
    };

    void Update();
    void Start(Mode mode);

    bool IsPlaying() const { return isPlaying_; }
    Mode GetMode() const { return mode_; }

private:
    uint32_t length_ = 60;
    float time_;
    Mode mode_;
    bool isPlaying_;
};