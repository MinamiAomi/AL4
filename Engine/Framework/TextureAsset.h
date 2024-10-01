#pragma once
#include "Asset.h"

class Texture;

class TextureAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Texture> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Texture> core_;
};