#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "TestObject.h"
#include "Audio/AudioSource.h"
#include "Door.h"
#include "Graphics/Skeleton.h"

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

    Door door_;
    TestObject testObject_;
    TestObject boxObject_;


    ModelInstance model_;
    std::shared_ptr<Skeleton> skeleton_;
    std::shared_ptr<Animation> walk_;
    float time_;
};
