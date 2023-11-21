#pragma once
#include "GPUResource.h"

#include <string>

#include "DescriptorHandle.h"

class GPUBuffer :
    public GPUResource {
public:
    size_t GetBufferSize() const { return bufferSize_; }

protected:
    void Create(const std::wstring& name, size_t bufferSize);
    void CreateConstantBuffer(const std::wstring& name, size_t dataSize);

    size_t bufferSize_ = 0;
};

class StructuredBuffer :
    public GPUBuffer {
public:
    void Create(const std::wstring& name, size_t numElements, size_t strideSize);

    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const {
        return D3D12_VERTEX_BUFFER_VIEW{
            .BufferLocation = resource_->GetGPUVirtualAddress(),
            .SizeInBytes = UINT(bufferSize_),
            .StrideInBytes = UINT(strideSize_)
        };
    }
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
        return D3D12_INDEX_BUFFER_VIEW{
            .BufferLocation = resource_->GetGPUVirtualAddress(),
            .SizeInBytes = UINT(bufferSize_),
            .Format = (strideSize_ == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT
        };
    }

    size_t GetNumElements() const { return numElements_; }
    size_t GetStrideSize() const { return strideSize_; }

protected:
    void CreateViews();

    size_t numElements_ = 0;
    size_t strideSize_ = 0;

    DescriptorHandle srvHandle_;
};

class ConstantBuffer : 
    public GPUBuffer {
public:
    void Create(const std::wstring& name, size_t bufferSize);

protected:
    void CreateView();

    DescriptorHandle cbvHandle_;
};