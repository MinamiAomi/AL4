#include "TLAS.h"

#include <vector>

#include "../Model.h"
#include "../Core/CommandContext.h"
#include "../Core/Graphics.h"

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)

void TLAS::Create(const std::wstring& name, CommandContext& commandContext) {

    auto graphics = Graphics::GetInstance();

    auto& instanceList = ModelInstance::GetInstanceList();

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;
    instanceDescs.reserve(instanceList.size());
    // レイトレで使用するオブジェクトをインスタンスデスクに登録
    for (auto& instance : instanceList) {
        if (!(instance->IsActive() && instance->GetModel())) {
            continue;
        }

        auto& desc = instanceDescs.emplace_back();

        for (uint32_t y = 0; y < 3; ++y) {
            for (uint32_t x = 0; x < 4; ++x) {
                desc.Transform[y][x] = instance->GetWorldMatrix().m[x][y];
            }
        }
        desc.InstanceID = instance->ReciveShadow() ? 1 : 0;
        desc.InstanceMask = PRIMARY_RAY_ATTRIBUTE;
        if (instance->CastShadow()) {
            desc.InstanceMask |= SHADOW_RAY_ATTRIBUTE;
        }
        desc.InstanceContributionToHitGroupIndex = 0;
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
        desc.AccelerationStructure = instance->GetModel()->GetBLAS().GetGPUVirtualAddress();
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS asInputs{};
    asInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    asInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    asInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    asInputs.NumDescs = UINT(instanceDescs.size());
    asInputs.InstanceDescs = commandContext.TransfarUploadBuffer(sizeof(instanceDescs[0]) * instanceDescs.size(), instanceDescs.data());

    // ASのサイズを取得
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO asInfo{};
    graphics->GetDXRDevoce()->GetRaytracingAccelerationStructurePrebuildInfo(&asInputs, &asInfo);

    CD3DX12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resultDesc = CD3DX12_RESOURCE_DESC::Buffer(asInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    CreateResource(name, defaultHeapProps, resultDesc, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
    // スクラッチリソース一時的なリソース
    GPUResource scratchResource;
    CD3DX12_RESOURCE_DESC scratchDesc = CD3DX12_RESOURCE_DESC::Buffer(asInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    scratchResource.CreateResource(L"TLAS ScratchDataBuffer", defaultHeapProps, scratchDesc, D3D12_RESOURCE_STATE_COMMON);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
    asDesc.Inputs = asInputs;
    asDesc.DestAccelerationStructureData = resource_->GetGPUVirtualAddress();
    asDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();

    commandContext.GetDXRCommandList()->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);
    // 生成完了までUAVバリアを張る
    commandContext.UAVBarrier(*this);
    // スクラッチリソースが解放されないようにする
    commandContext.TrackingObject(scratchResource.Get());

    // ビューを生成
    CreateView();
}

void TLAS::CreateView() {
    auto graphics = Graphics::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.RaytracingAccelerationStructure.Location = GetGPUVirtualAddress();

    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    graphics->GetDevice()->CreateShaderResourceView(nullptr, &srvDesc, srvHandle_);
}
