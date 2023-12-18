#include "ShaderTable.h"

#include <cassert>

#include "../Core/Helper.h"

UINT ShaderRecord::GetLocalRootArgumentsSize(const D3D12_ROOT_PARAMETER* parameters, UINT numParameters) {
    UINT localRootArgumentsSize = 0;
    for (UINT i = 0; i < numParameters; ++i) {
        auto& parameter = parameters[i];
        switch (parameter.ParameterType) {
        case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE: {
            localRootArgumentsSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS: {
            const UINT kuint32_tByteSize = sizeof(uint32_t);
            const UINT kConstantsAlignment = kuint32_tByteSize * 2;
            localRootArgumentsSize += Helper::AlignUp(kuint32_tByteSize * parameter.Constants.Num32BitValues, kConstantsAlignment);
            break;
        }
        case D3D12_ROOT_PARAMETER_TYPE_CBV:
        case D3D12_ROOT_PARAMETER_TYPE_SRV:
        case D3D12_ROOT_PARAMETER_TYPE_UAV: {
            localRootArgumentsSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS);
            break;
        }
        default: {
            assert(false);
            break;
        }
        }
    }
    return localRootArgumentsSize;
}

ShaderRecord::ShaderRecord(void* shaderIdentifierPtr, UINT shaderIdentifierSize, void* localRootArgumentsPtr, UINT localRootArgumentsSize) :
    shaderIdentifier_(shaderIdentifierPtr, shaderIdentifierSize),
    localRootArguments_(localRootArgumentsPtr, localRootArgumentsSize) {
}

void ShaderRecord::CopyTo(void* dest) const {
    BYTE* byteDest = static_cast<BYTE*>(dest);
    memcpy(byteDest, shaderIdentifier_.ptr, shaderIdentifier_.size);
    byteDest += shaderIdentifier_.size;
    if (localRootArguments_.ptr) {
        memcpy(byteDest, localRootArguments_.ptr, localRootArguments_.size);
    }
}

void ShaderTable::Create(const std::wstring& name, UINT shaderRecordSize, UINT numShaderRecords) {
    shaderRecordSize_ = Helper::AlignUp(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
    shaderRecords_.reserve(numShaderRecords);
    bufferSize_ = shaderRecordSize_ * numShaderRecords;
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize_);
    CreateResource(name, heapProps, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ);
    ASSERT_IF_FAILED(resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedShaderRecords_)));
    ZeroMemory(mappedShaderRecords_, bufferSize_);
}

void ShaderTable::Add(const ShaderRecord& shaderRecord) {
    assert(shaderRecords_.size() < shaderRecords_.capacity());
    shaderRecords_.emplace_back(shaderRecord);
    shaderRecord.CopyTo(mappedShaderRecords_);
    mappedShaderRecords_ += shaderRecordSize_;
}
