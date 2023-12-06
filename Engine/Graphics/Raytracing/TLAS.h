#pragma once
#include "../Core/GPUResource.h"

class StructuredBuffer;
class CommandContext;

class TLAS :
    public GPUResource {
public:
    void Create(const std::wstring& name, CommandContext& commandContext);

private:
    
    
};