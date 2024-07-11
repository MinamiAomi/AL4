#pragma once
#include "GameObject/Component.h"

#include "Graphics/Model.h"
#include "Externals/nlohmann/json.hpp"

class MeshComponent :
    public Component {
    COMPONENT_IMPL(MeshComponent);
public:
    void Initialize() override;
    void Update() override;
    void Edit() override;

    void SetModelName(const std::string& name) { modelName_ = name; }

private:
    ModelInstance model_;
    std::shared_ptr<PBRMaterial> customMaterial_;
    std::string modelName_;
};