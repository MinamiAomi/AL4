#pragma once
#include "GPUResource.h"

#include <string>

class GPUBuffer :
    public GPUResource {
public:
    void Create(const std::wstring& name, size_t bufferSize);
    void CreateVertexBuffer(const std::wstring& name, size_t numVertices, size_t vertexSize);
    void CreateIndexBuffer(const std::wstring& name, size_t numIndices, size_t indexSize);
    void CreateConstantBuffer(const std::wstring& name, size_t dataSize);

    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(size_t strideSize) const {
        return D3D12_VERTEX_BUFFER_VIEW{
            .BufferLocation = resource_->GetGPUVirtualAddress(),
            .SizeInBytes = UINT(bufferSize_),
            .StrideInBytes = UINT(strideSize)
        };
    }
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(size_t strideSize) const {
        return D3D12_INDEX_BUFFER_VIEW{
            .BufferLocation = resource_->GetGPUVirtualAddress(),
            .SizeInBytes = UINT(bufferSize_),
            .Format = (strideSize == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT
        };
    }

    size_t GetBufferSize() const { return bufferSize_; }

private:
    size_t bufferSize_ = 0;
};