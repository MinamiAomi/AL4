#pragma once

#include <memory>
#include <filesystem>

#include "Math/MathUtils.h"
#include "Core/GPUBuffer.h"
#include "Mesh.h"

class Model {
public:
    static std::shared_ptr<Model> Load(const std::filesystem::path& path);

private:
    Model() = default;
    ~Model() = default;

    std::vector<Mesh> meshes_;
};

class ModelInstance {
public:
    static const std::list<ModelInstance*>& GetInstanceList() { return instanceLists_; }


private:
    static std::list<ModelInstance*> instanceLists_;

    std::shared_ptr<Model> model_;
    Matrix4x4 worldMatrix_;
    Vector3 color_;
    float alpha_;

    bool isActive_;
};