#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/Model.h"

class TitleObject :
    public GameObject {
public:
    void Initialize();
    void Update(float time);

private:
    std::unique_ptr<ModelInstance> model_;
    Vector3 base_;
};