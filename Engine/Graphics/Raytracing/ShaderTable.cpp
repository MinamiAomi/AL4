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
