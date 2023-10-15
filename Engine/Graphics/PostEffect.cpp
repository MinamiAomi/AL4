#include "PostEffect.h"

#include "Core/Helper.h"
#include "Core/ShaderManager.h"
#include "Core/CommandContext.h"

const wchar_t kPostEffectVertexShadedr[] = L"Engine/Graphics/Shader/ScreenQuadVS.hlsl";
const wchar_t kPostEffectPixelShadedr[] = L"Engine/Graphics/Shader/PostEffectPS.hlsl";

void PostEffect::Initialize(const ColorBuffer& target) {
    CD3DX12_DESCRIPTOR_RANGE srvRange[1]{};
    srvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParameters[2]{};
    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsDescriptorTable(_countof(srvRange), srvRange);

    CD3DX12_STATIC_SAMPLER_DESC staticSampler[1]{};
    staticSampler->Init(0);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSampler);
    rootSignatureDesc.pStaticSamplers = staticSampler;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    rootSignature_.Create(L"PostEffect RootSignature", rootSignatureDesc);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_;

    auto shaderManager = ShaderManager::GetInstance();
    auto vs = shaderManager->Compile(kPostEffectVertexShadedr, ShaderManager::kVertex);
    auto ps = shaderManager->Compile(kPostEffectPixelShadedr, ShaderManager::kPixel);
    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
    pipelineStateDesc.BlendState = Helper::BlendDisable;
    pipelineStateDesc.RasterizerState = Helper::RasterizerNoCull;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = target.GetFormat();
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;

    pipelineState_.Create(L"PostEffect PipelineState", pipelineStateDesc);
}

void PostEffect::Render(CommandContext& commandContext, ColorBuffer& texture) {
    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(1, texture.GetSRV());
    commandContext.Draw(3);
}
