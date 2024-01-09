#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/Model.h"

class Floor :
    public GameObject {
public:
    void Initialize();
    void Update();

private:
    std::unique_ptr<ModelInstance> model_;

};
