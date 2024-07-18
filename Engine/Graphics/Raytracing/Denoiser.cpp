#include "Denoiser.h"

#include "../Core/ShaderManager.h"
#include "../Core/Helper.h"
#include "../Core/CommandContext.h"
#include "../Core/ColorBuffer.h"

void Denoiser::Initialize() {
    RootSignatureDescHelper rsDesc;
    rsDesc.AddDescriptorTable().AddSRVDescriptors(1, 0);
    rsDesc.AddDescriptorTable().AddUAVDescriptors(1, 0);
    rsDesc.AddDescriptorTable().AddUAVDescriptors(1, 1);
    rsDesc.AddConstants(1, 0);
    rootSignature_.Create(L"Denoiser", rsDesc);

    D3D12_COMPUTE_PIPELINE_STATE_DESC cpsDesc{};
    cpsDesc.pRootSignature = rootSignature_;
    auto cs = ShaderManager::GetInstance()->Compile(L"Raytracing/DenoiserCS.hlsl", ShaderType::Compute, 6, 6);
    cpsDesc.CS = CD3DX12_SHADER_BYTECODE(cs->GetBufferPointer(), cs->GetBufferSize());
    pipelineState_.Create(L"Denoiser", cpsDesc);
}

void Denoiser::Render(CommandContext& commandContext, ColorBuffer& intermadiateBuffer, ColorBuffer& accumulationBuffer, ColorBuffer& denoisedBuffer, uint32_t sampleCount) {
    commandContext.TransitionResource(intermadiateBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(accumulationBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.TransitionResource(denoisedBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.SetComputeRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetComputeDescriptorTable(0, intermadiateBuffer.GetSRV());
    commandContext.SetComputeDescriptorTable(1, accumulationBuffer.GetUAV());
    commandContext.SetComputeDescriptorTable(2, denoisedBuffer.GetUAV());
    commandContext.SetComputeConstants(3, sampleCount);
    commandContext.Dispatch(UINT((denoisedBuffer.GetWidth() + 31) / 32), UINT((denoisedBuffer.GetHeight() + 31) / 32));
    commandContext.UAVBarrier(accumulationBuffer);
    commandContext.UAVBarrier(denoisedBuffer);
    commandContext.FlushResourceBarriers();
}
