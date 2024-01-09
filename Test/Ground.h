#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

class Ground :
    public GameObject {
public:
    void Initialize();
    void Update();

private:
    std::unique_ptr<ModelInstance> model_;
    std::shared_ptr<DirectionalLight> sunLight_;
    std::unique_ptr<ModelInstance> skydomeModel_;

};
