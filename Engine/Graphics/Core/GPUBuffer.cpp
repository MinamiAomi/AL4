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

void GPUBuffer::CreateConstantBuffer(const std::wstring& name, size_t dataSize) {
    Create(name, Helper::AlignUp(dataSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
}


void StructuredBuffer::Create(const std::wstring& name, size_t numElements, size_t strideSize) {
    GPUBuffer::Create(name, numElements * strideSize);
    numElements_ = numElements;
    strideSize_ = strideSize;
    CreateViews();
}

void StructuredBuffer::CreateViews() {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = UINT(numElements_);
    srvDesc.Buffer.StructureByteStride = UINT(strideSize_);

    auto graphics = Graphics::GetInstance();
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}

void ConstantBuffer::Create(const std::wstring& name, size_t bufferSize) {
    GPUBuffer::Create(name, bufferSize);
    CreateView();
}

void ConstantBuffer::CreateView() {
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = resource_->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = UINT(bufferSize_);

    auto graphics = Graphics::GetInstance();
    if (cbvHandle_.IsNull()) {
        cbvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    device->CreateConstantBufferView(&cbvDesc, cbvHandle_);
}
