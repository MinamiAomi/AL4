#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "Audio/AudioSource.h"
#include "Graphics/Skeleton.h"
#include "GameObject/GameObjectManager.h"

#include "Player.h"

class TestScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Camera> camera_;
    Vector3 euler_;
    std::shared_ptr<DirectionalLight> sunLight_;

    static const uint32_t kRowCount = 2;
    static const uint32_t kColumnCount = 5;
    struct PBRSphere {
        ModelInstance model;
        std::shared_ptr<Material> material;
    };
    PBRSphere spheres_[kRowCount][kColumnCount];
    ModelInstance room_;

};
