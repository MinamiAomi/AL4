#include "TestRTRenderer.h"

#include "../Core/Graphics.h"
#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../Model.h"
#include "../DefaultTextures.h"
#include "../Core/SamplerManager.h"
#include "../ModelSorter.h"

namespace {
    static const wchar_t kRaytracingShader[] = L"Raytracing/Specular.hlsl";
    static const wchar_t kRayGenerationName[] = L"RayGeneration";
    static const wchar_t kRecursiveMissName[] = L"RecursiveMiss";
    static const wchar_t kRecursiveClosestHitName[] = L"RecursiveClosestHit";
    static const wchar_t kRecursiveHitGroupName[] = L"RecursiveHitGroup";
}

void TestRTRenderer::Create(uint32_t width, uint32_t height) {
    CreateRootSignature();
    CreateStateObject();
    CreateShaderTables();
    colorBuffer_.Create(L"TestRTRenderer", width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
}

void TestRTRenderer::Render(CommandContext& commandContext, const Camera& camera, const ModelSorter& modelSorter) {
    modelSorter;
    commandContext;
    camera;
}

void TestRTRenderer::CreateRootSignature() {

    RootSignatureDescHelper globalRootSignatureDesc;
    // Scene
    globalRootSignatureDesc.AddConstantBufferView(0);
    // TLAS
    globalRootSignatureDesc.AddShaderTable().AddSRVDescriptors(1, 0);
    // Skybox
    globalRootSignatureDesc.AddShaderTable().AddSRVDescriptors(1, 1);
    // BindlessTexture
    globalRootSignatureDesc.AddShaderTable().AddSRVDescriptors(BINDLESS_RESOURCE_MAX, 0, 2);
    // Color
    globalRootSignatureDesc.AddShaderTable().AddUAVDescriptors(1, 0);
    globalRootSignatureDesc.AddStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
    globalRootSignatureDesc.AddStaticSampler(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
    globalRootSignature_.Create(L"GlobalRootSignature", globalRootSignatureDesc);


    RootSignatureDescHelper localRootSignatureDesc;
    localRootSignatureDesc.SetFlag(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
    // VertexBuffere
    localRootSignatureDesc.AddShaderResourceView(0, 1);
    // IndexBuffer
    localRootSignatureDesc.AddShaderResourceView(1, 1);
    // Material
    localRootSignatureDesc.AddConstantBufferView(0, 1);
    hitGroupLocalRootSignature_.Create(L"HitGroupLocalRootSignature", localRootSignatureDesc);
}

void TestRTRenderer::CreateStateObject() {
    CD3DX12_STATE_OBJECT_DESC stateObjectDesc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    // 1.DXILLib
    auto shader = ShaderManager::GetInstance()->Compile(kRaytracingShader, ShaderType::Library, 6, 6);
    CD3DX12_SHADER_BYTECODE shaderByteCode(shader->GetBufferPointer(), shader->GetBufferSize());
    auto dxilLibSubobject = stateObjectDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    dxilLibSubobject->SetDXILLibrary(&shaderByteCode);
    dxilLibSubobject->DefineExport(kRayGenerationName);
    dxilLibSubobject->DefineExport(kRecursiveClosestHitName);
    dxilLibSubobject->DefineExport(kRecursiveMissName);

    // 2.ヒットグループ
    auto hitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hitGroup->SetClosestHitShaderImport(kRecursiveClosestHitName);
    hitGroup->SetHitGroupExport(kRecursiveHitGroupName);
    hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 3.ヒットグループのローカルルートシグネチャ
    auto hitGroupRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
    hitGroupRootSignature->SetRootSignature(hitGroupLocalRootSignature_);

    // 4.ヒットグループアソシエーション
    auto hitGroupRootSignatureAssociation = stateObjectDesc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
    hitGroupRootSignatureAssociation->SetSubobjectToAssociate(*hitGroupRootSignature);
    hitGroupRootSignatureAssociation->AddExport(kRecursiveHitGroupName);

    // 6.シェーダーコンフィグ
    auto shaderConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    size_t maxPayloadSize = 3 * sizeof(float) + 1 * sizeof(uint32_t);      // 最大ペイロードサイズ
    size_t maxAttributeSize = 2 * sizeof(float);   // 最大アトリビュートサイズ
    shaderConfig->Config((UINT)maxPayloadSize, (UINT)maxAttributeSize);

    // 7.パイプラインコンフィグ
    auto pipelineConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    uint32_t maxTraceRecursionDepth = 2; // 再帰回数1回
    pipelineConfig->Config(maxTraceRecursionDepth);

    // 8.グローバルルートシグネチャ
    auto globalRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(globalRootSignature_);

    stateObject_.Create(L"RaytracingStateObject", stateObjectDesc);
}

void TestRTRenderer::CreateShaderTables() {
    {
        Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
        stateObject_.Get().As(&stateObjectProperties);

        auto InsertIdentifier = [&](const wchar_t* name) {
            identifierMap_[name] = stateObjectProperties->GetShaderIdentifier(name);
        };
        InsertIdentifier(kRayGenerationName);
        InsertIdentifier(kRecursiveHitGroupName);
        InsertIdentifier(kRecursiveMissName);
    }

    {
        ShaderRecord rayGenerationShaderRecord(identifierMap_[kRayGenerationName]);
        rayGenerationShaderTable_.Create(L"RayGenerationShaderTable", &rayGenerationShaderRecord, 1);
    }
    // ヒットグループは毎フレーム更新
    {
        ShaderRecord missShaderRecord(identifierMap_[kRecursiveMissName]);
        missShaderTable_.Create(L"MissShaderTable", &missShaderRecord, 1);
    }
}

void TestRTRenderer::BuildScene(CommandContext& commandContext, const ModelSorter& modelSorter) {

    struct MaterialData {
        Vector3 albedo;
        float metallic;
        float roughness;
        uint32_t albedoMapIndex;
        uint32_t metallicRoughnessMapIndex;
        uint32_t normalMapIndex;
    };

    uint32_t defaultWhiteTextureIndex = DefaultTexture::White.GetSRV().GetIndex();
    uint32_t defaultNormalTextureIndex = DefaultTexture::Normal.GetSRV().GetIndex();

    auto ErrorMaterial = [defaultWhiteTextureIndex, defaultNormalTextureIndex]() {
        MaterialData materialData;
        materialData.albedo = { 0.988f, 0.059f, 0.753f };
        materialData.metallic = 0.0f;
        materialData.roughness = 0.0f;
        materialData.albedoMapIndex = defaultWhiteTextureIndex;
        materialData.metallicRoughnessMapIndex = defaultWhiteTextureIndex;
        materialData.normalMapIndex = defaultNormalTextureIndex;
        return materialData;
    };

    auto SetMaterialData = [](MaterialData& dest, const PBRMaterial& src) {
        dest.albedo = src.albedo;
        dest.metallic = src.metallic;
        dest.roughness = src.roughness;
        if (src.albedoMap) { dest.albedoMapIndex = src.albedoMap->GetSRV().GetIndex(); }
        if (src.metallicRoughnessMap) { dest.metallicRoughnessMapIndex = src.metallicRoughnessMap->GetSRV().GetIndex(); }
        if (src.normalMap) { dest.normalMapIndex = src.normalMap->GetSRV().GetIndex(); }
    };


    auto& drawModels = modelSorter.GetDrawModels();

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;
    instanceDescs.reserve(drawModels.size());

    size_t numShaderRecords = 0;
    for (auto& instance : drawModels) {
        numShaderRecords += instance->GetModel()->GetMeshes().size();
    }

    std::vector<ShaderRecord> shaderRecords;
    shaderRecords.reserve(numShaderRecords);

    auto hitGroupIdentifier = identifierMap_[kRecursiveHitGroupName];

    // レイトレで使用するオブジェクトをインスタンスデスクに登録
    for (auto& instance : drawModels) {

        auto model = instance->GetModel();

        auto& desc = instanceDescs.emplace_back();

        for (uint32_t y = 0; y < 3; ++y) {
            for (uint32_t x = 0; x < 4; ++x) {
                desc.Transform[y][x] = instance->GetWorldMatrix().m[x][y];
            }
        }
        desc.InstanceID = 0;
        desc.InstanceMask = 0xFF;
        desc.InstanceContributionToHitGroupIndex = (UINT)shaderRecords.size();
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
        desc.AccelerationStructure = model->GetBLAS().GetGPUVirtualAddress();

        auto instanceMaterial = instance->GetMaterial();

        for (auto& mesh : model->GetMeshes()) {
            auto& shaderRecord = shaderRecords.emplace_back(hitGroupIdentifier);

            D3D12_GPU_VIRTUAL_ADDRESS vb = model->GetVertexBuffer().GetGPUVirtualAddress();
            D3D12_GPU_VIRTUAL_ADDRESS ib = model->GetIndexBuffer().GetGPUVirtualAddress();
            vb += model->GetVertexBuffer().GetElementSize() * mesh.vertexOffset;
            ib += model->GetIndexBuffer().GetElementSize() * mesh.indexOffset;

            shaderRecord.Add(vb);
            shaderRecord.Add(ib);

            MaterialData materialData = ErrorMaterial();
            // インスタンスのマテリアルを優先
            if (instanceMaterial) {
                SetMaterialData(materialData, *instanceMaterial);
            }
            // メッシュのマテリアル
            else if (mesh.material < model->GetMaterials().size()) {
                SetMaterialData(materialData, model->GetMaterials()[mesh.material]);
            }

            D3D12_GPU_VIRTUAL_ADDRESS materialCB = commandContext.TransfarUploadBuffer(sizeof(materialData), &materialData);
            shaderRecord.Add(materialCB);
        }
    }


    hitGroupShaderTable_.Create(L"RaytracingRenderer HitGroupShaderTable", shaderRecords.data(), (UINT)shaderRecords.size());
    tlas_.Create(L"RaytracingRenderer TLAS", commandContext, instanceDescs.data(), instanceDescs.size());
}
