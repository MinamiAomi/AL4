#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <string>

class StateObject {
public:
    void Create(const std::wstring& name, const D3D12_STATE_OBJECT_DESC& desc);

    operator ID3D12StateObject* () const { return stateObject_.Get(); }
    operator bool() const { return stateObject_; }

    const Microsoft::WRL::ComPtr<ID3D12StateObject>& Get() const { return stateObject_; }

private:
    Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject_;
};