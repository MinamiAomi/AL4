#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class GPUResource {
    friend class CommandContext;
public:
    ~GPUResource() { Destroy(); }

    operator ID3D12Resource* () const { return resource_.Get(); }

    const Microsoft::WRL::ComPtr<ID3D12Resource>& Get() const { return resource_; }

    ID3D12Resource* operator->() { resource_.Get(); }
    const ID3D12Resource* operator->() const { resource_.Get(); }

    ID3D12Resource** GetAddressOf() { return resource_.GetAddressOf(); }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

    virtual void Destroy() {
        resource_ = nullptr;
    }

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_STATES state_ = D3D12_RESOURCE_STATE_COMMON;
};