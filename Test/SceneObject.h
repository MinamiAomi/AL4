#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Externals/nlohmann/json.hpp"
#include "Graphics/Model.h"

class SceneObject :
    public GameObject {

    friend void to_json(nlohmann::json& json, const SceneObject& value);
    friend void from_json(const nlohmann::json& json, SceneObject& value);

public:
    void Initialize();

private:
    std::unique_ptr<ModelInstance> model_;

};

