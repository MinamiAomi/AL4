#pragma once
#include "../Core/GPUResource.h"
#include "../Core/DescriptorHandle.h"

class StructuredBuffer;
class CommandContext;

class TLAS :
    public GPUResource {
public:
    void Create(const std::wstring& name, CommandContext& commandContext);

    const DescriptorHandle& GetSRV() const { return srvHandle_; }

private:
    void CreateView();

    DescriptorHandle srvHandle_;
    
};
