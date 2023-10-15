#include "GPUBuffer.h"

#include <cassert>

#include "Helper.h"
#include "Graphics.h"

void GPUBuffer::Create(const std::wstring& name, size_t bufferSize) {
    auto device = Graphics::GetInstance()->GetDevice();

    Destroy();

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));

    state_ = D3D12_RESOURCE_STATE_COMMON;
    bufferSize_ = bufferSize;

    D3D12_OBJECT_SET_NAME(resource_, name.c_str());
}

void GPUBuffer::CreateVertexBuffer(const std::wstring& name, size_t numVertices, size_t vertexSize) {
    Create(name, numVertices * vertexSize);
}

void GPUBuffer::CreateIndexBuffer(const std::wstring& name, size_t numIndices, size_t indexSize) {
    Create(name, numIndices * indexSize);
}

void GPUBuffer::CreateConstantBuffer(const std::wstring& name, size_t dataSize) {
    Create(name, Helper::AlignUp(dataSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
}
