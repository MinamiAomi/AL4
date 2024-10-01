#pragma once
#include "Asset.h"

class Material;

class MaterialAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Material> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Material> core_;
};