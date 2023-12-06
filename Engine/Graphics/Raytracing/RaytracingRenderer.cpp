#include "RaytracingRenderer.h"

#include "../Core/Graphics.h"
#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"

static const wchar_t kRayGenerationName[] = L"RayGeneration";
static const wchar_t kMissName[] = L"Miss";
static const wchar_t kPrimaryRayClosestHitName[] = L"PrimaryRayClosestHit";
static const wchar_t kShadowRayClosestHitName[] = L"ShadowRayClosestHit";
static const wchar_t kPrimaryRayHitGroupName[] = L"PrimaryRayHitGroup";
static const wchar_t kShadowRayHitGroupName[] = L"ShadowRayHitGroup";

void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc) {
    std::wstringstream wstr;
    wstr << L"\n";
    wstr << L"--------------------------------------------------------------------\n";
    wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
    if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
    if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

    auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports) {
        std::wostringstream woss;
        for (UINT i = 0; i < numExports; i++) {
            woss << L"|";
            if (depth > 0) {
                for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
            }
            woss << L" [" << i << L"]: ";
            if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
            woss << exports[i].Name << L"\n";
        }
        return woss.str();
    };

    for (UINT i = 0; i < desc->NumSubobjects; i++) {
        wstr << L"| [" << i << L"]: ";
        switch (desc->pSubobjects[i].Type) {
        case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
            wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
            wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
            wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
        {
            wstr << L"DXIL Library 0x";
            auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
            wstr << ExportTree(1, lib->NumExports, lib->pExports);
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
        {
            wstr << L"Existing Library 0x";
            auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << collection->pExistingCollection << L"\n";
            wstr << ExportTree(1, collection->NumExports, collection->pExports);
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
        {
            wstr << L"Subobject to Exports Association (Subobject [";
            auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
            UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
            wstr << index << L"])\n";
            for (UINT j = 0; j < association->NumExports; j++) {
                wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
            }
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
        {
            wstr << L"DXIL Subobjects to Exports Association (";
            auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
            wstr << association->SubobjectToAssociate << L")\n";
            for (UINT j = 0; j < association->NumExports; j++) {
                wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
            }
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
        {
            wstr << L"Raytracing Shader Config\n";
            auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
            wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
            wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
        {
            wstr << L"Raytracing Pipeline Config\n";
            auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
            wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
        {
            wstr << L"Hit Group (";
            auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
            wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport ? hitGroup->AnyHitShaderImport : L"[none]") << L"\n";
            wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport ? hitGroup->ClosestHitShaderImport : L"[none]") << L"\n";
            wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport ? hitGroup->IntersectionShaderImport : L"[none]") << L"\n";
            break;
        }
        }
        wstr << L"|--------------------------------------------------------------------\n";
    }
    wstr << L"\n";
    OutputDebugStringW(wstr.str().c_str());
}

void RaytracingRenderer::Create() {
    CreateRootSignature();
    CreateStateObject();
}

void RaytracingRenderer::Render(CommandContext& commandContext, const Camera& camera) {
    auto commandList = commandContext.GetDXRCommandList();
    
    commandList->SetComputeRootSignature(globalRootSignature_);
    commandList->SetPipelineState1(stateObject_);
    //commandList->DispatchRays();
}

void RaytracingRenderer::CreateRootSignature() {

    {
        CD3DX12_ROOT_PARAMETER rootParameters[1]{};
        rootParameters[0].InitAsConstantBufferView(0);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        globalRootSignature_.Create(L"GlobalRootSignatures", rootSignatureDesc);
    }


    // 一次レイヒットグループ
    {
        CD3DX12_ROOT_PARAMETER rootParameters[1]{};
        rootParameters[0].InitAsConstants(1, 0, 1);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

        primaryRayHitGroupLocalRootSignature_.Create(L"PrimaryRayHitGroupLocalRootSignature", rootSignatureDesc);
    }
}

void RaytracingRenderer::CreateStateObject() {
    CD3DX12_STATE_OBJECT_DESC stateObjectDesc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    // 1.DXILLib
    auto shader = ShaderManager::GetInstance()->Compile(L"Raytracing/Raytracing.hlsl", ShaderType::Library, 6, 6);
    CD3DX12_SHADER_BYTECODE shaderByteCode(shader->GetBufferPointer(), shader->GetBufferSize());
    auto dxilLibSubobject = stateObjectDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    dxilLibSubobject->SetDXILLibrary(&shaderByteCode);
    dxilLibSubobject->DefineExport(kRayGenerationName);
    dxilLibSubobject->DefineExport(kMissName);
    dxilLibSubobject->DefineExport(kPrimaryRayClosestHitName);
    dxilLibSubobject->DefineExport(kShadowRayClosestHitName);

    // 2.一次レイヒットグループ
    auto primaryRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    primaryRayHitGroup->SetClosestHitShaderImport(kPrimaryRayClosestHitName);
    primaryRayHitGroup->SetHitGroupExport(kPrimaryRayHitGroupName);
    primaryRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 3.一次レイヒットグループローカルルートシグネチャ
    auto primaryRayHitGroupRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
    primaryRayHitGroupRootSignature->SetRootSignature(primaryRayHitGroupLocalRootSignature_);

    // 4.一次レイヒットグループアソシエーション
    auto primaryRayHitGroupAssociation = stateObjectDesc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
    primaryRayHitGroupAssociation->SetSubobjectToAssociate(*primaryRayHitGroupRootSignature);
    primaryRayHitGroupAssociation->AddExport(kPrimaryRayHitGroupName);
  
    // 5.シャドウレイヒットグループ
    auto shadowRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    shadowRayHitGroup->SetClosestHitShaderImport(kShadowRayClosestHitName);
    shadowRayHitGroup->SetHitGroupExport(kShadowRayHitGroupName);
    shadowRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 6.シェーダーコンフィグ
    auto shaderConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    uint32_t maxPayloadSize = static_cast<uint32_t>(sizeof(uint32_t));      // 最大ペイロードサイズ
    uint32_t maxAttributeSize = 2 * static_cast<uint32_t>(sizeof(float));   // 最大アトリビュートサイズ
    shaderConfig->Config(maxPayloadSize, maxAttributeSize);

    // 7.パイプラインコンフィグ
    auto pipelineConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    uint32_t maxTraceRecursionDepth = 2; // 一次レイ, シャドウレイ
    pipelineConfig->Config(maxTraceRecursionDepth);

    // 8.グローバルルートシグネチャ
    auto globalRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(globalRootSignature_);

    //PrintStateObjectDesc(stateObjectDesc);

    stateObject_.Create(L"RaytracingStateObject", stateObjectDesc);
}
