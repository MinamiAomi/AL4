#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Core/GPUBuffer.h"
#include "Math/MathUtils.h"
#include "Material.h"

class CommandContext;

struct Mesh {
    struct Vertex {
        Vector3 position;
        uint32_t normal;
        uint32_t tangent;
        Vector2 texcood;
    };

    using Index = uint32_t;

    StructuredBuffer vertexBuffer;
    StructuredBuffer indexBuffer;

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    std::shared_ptr<PBRMaterial> material;

    void CreateBuffers(CommandContext& commandContext);
};