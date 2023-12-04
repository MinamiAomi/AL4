#pragma once
#include "../Core/GPUResource.h"

class StructuredBuffer;
class CommandContext;

class BLAS : 
    public GPUResource {
public:
    void Create(const std::wstring& name, CommandContext& commandContext, StructuredBuffer& vertexBuffer, StructuredBuffer& indexBuffer);
};