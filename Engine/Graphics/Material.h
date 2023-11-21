#pragma once

#include <memory>
#include <string>

#include "Math/MathUtils.h"

class Material {
public:

    void SetBaseColor(const Vector3& baseColor) { baseColor_ = baseColor; }
    void SetMetallic(float metallic) { metallic_ = metallic; }
    void SetRoughness(float roughness) { roughness_ = roughness; }

    const Vector3& GetBaseColor() { return baseColor_; }
    float GetMetallic() { return metallic_; }
    float GetRoughness() { return roughness_; }

private:
    Vector3 baseColor_;
    float metallic_;
    float roughness_;
    std::shared_ptr<Texture> baseColorMap_;
    std::shared_ptr<Texture> Map_;
    std::shared_ptr<Texture> baseColorMap_;
};