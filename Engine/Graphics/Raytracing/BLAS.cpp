#include "BLAS.h"


void BLAS::Create(const std::wstring& name, StructuredBuffer& vertexBuffer, StructuredBuffer& indexBuffer) {
    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc{};
    // 三角形
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    // 不透明
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    // メッシュをセット
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer.GetGPUVirtualAddress();;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexBuffer.GetElementSize();
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.GetNumElements();
    geometryDesc.Triangles.IndexBuffer = indexBuffer.GetGPUVirtualAddress();
    geometryDesc.Triangles.IndexFormat = indexBuffer.GetElementSize() == sizeof(UINT) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    geometryDesc.Triangles.IndexCount = indexBuffer.GetNumElements();

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};

    auto& asInputs = asDesc.Inputs;
    asInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    asInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    // 容量を小さく
    asInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
    // トレースを早く
    asInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    asInputs.NumDescs = 1;
    asInputs.pGeometryDescs = &geometryDesc;
    asInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

    name;
}
