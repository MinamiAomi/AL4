#include "LightingRenderingPass.h"

#include "Core/Helper.h"
#include "Core/Graphics.h"
#include "Core/ShaderManager.h"
#include "Core/CommandContext.h"
#include "Math/Camera.h"
#include "Core/SamplerManager.h"
#include "GeometryRenderingPass.h"

namespace {
    const wchar_t kVertexShader[] = L"ScreenQuadVS.hlsl";
    const wchar_t kPixelShader[] = L"Standard/LightingPassPS.hlsl";
}

void LightingRenderingPass::Initialize(uint32_t width, uint32_t height) {
    float clearColor[] = { 0.04f, 0.1f, 0.6f, 1.0f };
    result_.SetClearColor(clearColor);
    result_.Create(L"LightingRenderingPass Result", width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

    // ルートシグネチャ
    {
        CD3DX12_DESCRIPTOR_RANGE albedoRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE metallicRoughnessRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        CD3DX12_DESCRIPTOR_RANGE normalRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        CD3DX12_DESCRIPTOR_RANGE depthRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

        CD3DX12_ROOT_PARAMETER rootParameters[RootIndex::NumRootParameters]{};
        rootParameters[RootIndex::Scene].InitAsConstantBufferView(0);
        rootParameters[RootIndex::Albedo].InitAsDescriptorTable(1, &albedoRange);
        rootParameters[RootIndex::MetallicRoughness].InitAsDescriptorTable(1, &metallicRoughnessRange);
        rootParameters[RootIndex::Normal].InitAsDescriptorTable(1, &normalRange);
        rootParameters[RootIndex::Depth].InitAsDescriptorTable(1, &depthRange);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc[1]{};
        staticSamplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = _countof(staticSamplerDesc);
        rootSignatureDesc.pStaticSamplers = staticSamplerDesc;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignature_.Create(L"LightingRenderingPass RootSignature", rootSignatureDesc);
    }

    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
        pipelineStateDesc.pRootSignature = rootSignature_;

        auto shaderManager = ShaderManager::GetInstance();
        auto vs = shaderManager->Compile(kVertexShader, ShaderManager::kVertex);
        auto ps = shaderManager->Compile(kPixelShader, ShaderManager::kPixel);
        pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        pipelineStateDesc.BlendState = Helper::BlendDisable;
        pipelineStateDesc.RasterizerState = Helper::RasterizerNoCull;
        pipelineStateDesc.NumRenderTargets = 1;
        pipelineStateDesc.RTVFormats[0] = result_.GetRTVFormat();
        pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc.SampleDesc.Count = 1;

        pipelineState_.Create(L"LightingRenderingPass PipelineState", pipelineStateDesc);
    }

}

void LightingRenderingPass::Render(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass, const Camera& camera, const DirectionalLight& light) {

    struct SceneData {
        Matrix4x4 viewProjectionInverseMatrix;
        Vector3 cameraPosition;
        float pad1;
        Vector3 lightColor;
        float pad2;
        Vector3 lightDirection;
    };

    commandContext.TransitionResource(geometryRenderingPass.GetAlbedo(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(geometryRenderingPass.GetMetallicRoughness(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(geometryRenderingPass.GetNormal(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(geometryRenderingPass.GetDepth(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(result_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.FlushResourceBarriers();
    
    commandContext.ClearColor(result_);
    commandContext.SetRenderTarget(result_.GetRTV());
    commandContext.SetViewportAndScissorRect(0, 0, result_.GetWidth(), result_.GetHeight());

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    SceneData sceneData;
    sceneData.viewProjectionInverseMatrix = camera.GetViewProjectionMatrix().Inverse();
    sceneData.cameraPosition = camera.GetPosition();
    sceneData.lightColor = light.color;
    sceneData.lightDirection = light.direction;
    commandContext.SetDynamicConstantBufferView(RootIndex::Scene, sizeof(sceneData), &sceneData);
    commandContext.SetDescriptorTable(RootIndex::Albedo, geometryRenderingPass.GetAlbedo().GetSRV());
    commandContext.SetDescriptorTable(RootIndex::MetallicRoughness, geometryRenderingPass.GetMetallicRoughness().GetSRV());
    commandContext.SetDescriptorTable(RootIndex::Normal, geometryRenderingPass.GetNormal().GetSRV());
    commandContext.SetDescriptorTable(RootIndex::Depth, geometryRenderingPass.GetDepth().GetSRV());
    commandContext.Draw(3);
}
