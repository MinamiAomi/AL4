#include "Door.h"

#include <algorithm>

#include "Framework/ResourceManager.h"

std::string ToLower(std::string str) {
    for (auto& c : str) {
        if (c >= 'A' && c <= 'Z') {
            c += 32;
        }
    }
    return str;
}

const char* Door::Part::kNames[] = {
    "Frame",
    "Panel",
    "Knob"
};

void Door::Initialize() {
    auto rm = ResourceManager::GetInstance();

    for (size_t i = 0; i < Part::NumParts; ++i) {
        std::string rsName = std::string("door_") + ToLower(Part::kNames[i]);
        parts_[i].model.SetModel(rm->FindModel(rsName));
        parts_[i].animation = rm->FindAnimation(rsName);
        parts_[i].transform.SetParent(&transform_);
    }

    animationTime_ = 0.0f;
}

void Door::Update() {

    auto& anim = parts_[0].animation->GetAnimation("FrameAction");
    animationTime_ += 1.0f / 60.0f;
    animationTime_ = std::fmod(animationTime_, anim.duration);
    for (size_t i = 0; i < Part::NumParts; ++i) {
        auto& nodeAnim = parts_[i].animation->GetAnimation(Part::kNames[i] + std::string("Action")).nodeAnimations[Part::kNames[i]];
        parts_[i].transform.translate = CalculateValue(nodeAnim.translate, animationTime_);
        parts_[i].transform.rotate = CalculateValue(nodeAnim.rotate, animationTime_);
        parts_[i].transform.scale = CalculateValue(nodeAnim.scale, animationTime_);
    }

    transform_.UpdateMatrix();
    for (size_t i = 0; i < Part::NumParts; ++i) {
        parts_[i].transform.UpdateMatrix();
        parts_[i].model.SetWorldMatrix(parts_[i].transform.worldMatrix);
    }
}
