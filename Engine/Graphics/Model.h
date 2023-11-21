#pragma once

#include <memory>

#include "Math/MathUtils.h"
#include "Core/GPUBuffer.h"
#include "Mesh.h"

class Model {
public:

private:
    std::vector<Mesh> meshes_;

};

class ModelInstance {
public:


private:
    std::shared_ptr<Model> model_;
    Matrix4x4 worldMatrix_;
    Vector3 color_;
    float alpha_;

    std::unique_ptr<GPUBuffer> boneData_;

    bool isActive_;
};