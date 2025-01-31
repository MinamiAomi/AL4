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
#include "Graphics/Sprite.h"

class TestScene :
    public LIEngine::BaseScene {
public:

    /// <summary>
    /// 初期化
    /// </summary>
    void OnInitialize() override;
    /// <summary>
    /// 更新
    /// </summary>
    void OnUpdate() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void OnFinalize() override;

private:
    std::shared_ptr<LIEngine::DirectionalLight> sunLight_;

    static const uint32_t kRowCount = 2;
    static const uint32_t kColumnCount = 5;
    struct PBRSphere {
        LIEngine::ModelInstance model;
        std::shared_ptr<LIEngine::Material> material;
    };
    PBRSphere spheres_[kRowCount][kColumnCount];
    LIEngine::ModelInstance room_;
    LIEngine::Sprite sprite_;
};
