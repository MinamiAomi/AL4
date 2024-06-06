#pragma once
#include "GameObject/Component.h"

#include "Graphics/Model.h"
#include "Externals/nlohmann/json.hpp"

class MeshComponent :
    public Component {
public:
    void Initialize() override;

    void SetModelName(const std::string& name) { modelName_ = name; }

private:
    ModelInstance model_;
    std::string modelName_;
};