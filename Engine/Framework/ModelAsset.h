#pragma once
#include "Asset.h"

class Model;

class ModelAsset :
    public Asset {
public:
    void RenderInInspectorView() override;

    std::shared_ptr<Model> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Model> core_;
};