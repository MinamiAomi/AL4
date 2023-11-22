#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Core/GPUBuffer.h"
#include "Math/MathUtils.h"

class CommandContext;
struct Material;

struct Mesh {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texcood;
    };

    using Index = uint32_t;

    StructuredBuffer vertexBuffer;
    StructuredBuffer indexBuffer;

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    std::shared_ptr<Material> material_;

    void CreateBuffers(CommandContext& commandContext);
};