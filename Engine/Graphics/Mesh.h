#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Core/GPUBuffer.h"
#include "Math/MathUtils.h"

class Mesh {
public:
    struct SurfaceVertex {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector2 texcood;
    };

    struct SkinVertex {
        uint32_t boneIndices[4];
        float weights[4];
    };

    using Index = uint32_t;



private:
    std::string name_;
    std::unique_ptr<GPUBuffer> surfaceVertexBuffer_;
    std::unique_ptr<GPUBuffer> skinVertexBuffer_;
    std::unique_ptr<GPUBuffer> indexBuffer_;
    std::vector<SurfaceVertex> surfaceVertices_;
    std::vector<SkinVertex> skinVertices_;
    std::vector<Index> indices_;
};