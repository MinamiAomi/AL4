#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

#include "TitleObject.h"

class TitleScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<DirectionalLight> sunLight_;

    std::shared_ptr<TitleObject> titleObject_;
    std::unique_ptr<ModelInstance> panel_;

    uint32_t cycle_;
    float time_;
};
