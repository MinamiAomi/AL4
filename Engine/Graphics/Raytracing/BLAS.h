#pragma once
#include "../Core/GPUBuffer.h"

class BLAS : 
    public GPUBuffer {
public:
    void Create(const std::wstring& name, StructuredBuffer& vertexBuffer, StructuredBuffer& indexBuffer);

private:

};