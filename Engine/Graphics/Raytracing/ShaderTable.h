#pragma once
#include "../Core/GPUResource.h"

#include <vector>

class ShaderRecord {
public:
    static UINT GetLocalRootArgumentsSize(const D3D12_ROOT_PARAMETER* parameters, UINT numParameters);

    ShaderRecord(
        void* shaderIdentifierPtr,
        UINT shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
        void* localRootArgumentsPtr = nullptr,
        UINT localRootArgumentsSize = 0);

    void CopyTo(void* dest) const;

private:
    struct PointerSize {
        void* ptr;
        UINT size;
        PointerSize(void* ptr, UINT size) : ptr(ptr), size(size) {}
    };

    PointerSize shaderIdentifier_;
    PointerSize localRootArguments_;
};

class ShaderTable :
    public GPUResource {
public:
    void Create(const std::wstring& name, UINT shaderRecordSize, UINT numShaderRecords);

private:
    BYTE* mappedShaderRecords_;
    UINT shaderRecordSize_;
    std::vector<ShaderRecord> shaderRecords_;
};