#pragma once

#include "Graphics/Animation.h"
#include "Graphics/Model.h"


class Door {
public:
    struct Part {
        enum Type {
            Frame,
            Panel,
            Knob,

            NumParts
        };
        static const char* kNames[];
        Transform transform;
        ModelInstance model;
        std::shared_ptr<Animation> animation;
    };

    void Initialize();
    void Update();

private:
    Transform transform_;
    Part parts_[Part::NumParts];
    float animationTime_;
};