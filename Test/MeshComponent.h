#pragma once
#include "GameObject/Component.h"

#include "Framework/ModelAsset.h"
#include "Graphics/Model.h"

class MeshComponent :
    public Component {
    COMPONENT_IMPL(MeshComponent);
public:
    void Initialize() override;
    void Update() override;
    void Edit() override;
    void Export(nlohmann::json& json) const override;
    void Import(const nlohmann::json& json) override;

    void SetModelName(const std::string& name) { modelName_ = name; }

private:
    void ApplyModel();

    ModelInstance model_;
    std::shared_ptr<Material> customMaterial_;
    std::shared_ptr<ModelAsset> asset_;
    std::string modelName_;
};