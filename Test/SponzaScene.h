#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

#include "DebugCamera.h"
#include "TestObject.h"

class SponzaScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<DebugCamera> debugCamera_;
    std::shared_ptr<DirectionalLight> sunLight_;

    std::vector<std::shared_ptr<TestObject>> testObjects_;

    struct ParticleConstant {
        int32_t spawnCount = 4;
        int32_t spawnCycle = 1;
        float speed = 0.01f;
    } particleConstant_;
    int32_t particleTimer_;

    Random::RandomNumberGenerator rng_;
};
