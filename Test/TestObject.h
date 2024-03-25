#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/Model.h"

class TestObject :
    public GameObject {
public:
    void Initialize(const std::string& name, const Vector3& position);
    void Update();
    void SetRotate(bool rotate) { rotate_ = rotate; }
    void DrawImGui(const std::string& name);

private:
    std::unique_ptr<ModelInstance> model_;
    bool rotate_ = true;
};