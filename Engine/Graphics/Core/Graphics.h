#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHandle.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"

#define BINDLESS_RESOURCE_MAX 1024

class Graphics {
public:
    static Graphics* GetInstance();

    void Initialize();
    void Finalize();

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

    ID3D12Device* GetDevice() const { return device_.Get(); }
    CommandQueue& GetCommandQueue() { return commandQueue_; }
    DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return descriptorHeaps_[type]; }

private:
    static const uint32_t kNumRTVs = 16;
    static const uint32_t kNumDSVs = 2;
    static const uint32_t kNumSRVs = BINDLESS_RESOURCE_MAX;
    static const uint32_t kNumSamplers = 16;

    Graphics() = default;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void CreateDevice();

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    CommandQueue commandQueue_;
    DescriptorHeap descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    
};