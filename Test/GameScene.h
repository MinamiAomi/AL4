#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Math/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/ToonModel.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Camera> camera_;

    std::shared_ptr<Model> floor_;
    std::shared_ptr<Model> teapot_;
    std::shared_ptr<Model> bunny_;

    ModelInstance floorModel_;
    ModelInstance teapotModel_;
    ModelInstance bunnyModel_;
};
