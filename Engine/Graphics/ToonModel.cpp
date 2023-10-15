#include "ToonModel.h"

#include "Core/Graphics.h"
#include "Core/CommandContext.h"
#include "Core/SamplerManager.h"
#include "ToonRenderer.h"
#include "DefaultTextures.h"

void ToonModel::Create(const ModelData& modelData) {

    struct MaterialData {
        Vector3 diffuse;
        float pad;
        Vector3 specular;
    };

    std::vector<std::shared_ptr<Texture>> createdTextures;
    std::vector<std::shared_ptr<Material>> createdMaterials;

    CommandContext commandContext;
    commandContext.Create();

    for (auto& srcMesh : modelData.meshes) {
        auto& srcMaterial = modelData.materials[srcMesh.materialIndex];
        const ModelData::Texture* srcTexture = nullptr;
        if (!modelData.textures.empty()) {
            srcTexture = &modelData.textures[srcMaterial.textureIndex];
        }

        // 生成するメッシュ
        auto& destMesh = meshes_.emplace_back();
        // 頂点バッファを作成
        destMesh.vertexBuffer.Create(L"ToonModel VertexBuffer", sizeof(srcMesh.vertices[0]), srcMesh.vertices.size());
        destMesh.vertexBuffer.Copy(srcMesh.vertices.data(), destMesh.vertexBuffer.GetBufferSize());
        // インデックスバッファを作成
        destMesh.indexCount = uint32_t(srcMesh.indices.size());
        destMesh.indexBuffer.Create(L"ToonModel indexBuffer", sizeof(srcMesh.indices[0]), srcMesh.indices.size());
        destMesh.indexBuffer.Copy(srcMesh.indices.data(), destMesh.indexBuffer.GetBufferSize());
        // マテリアル
        if (srcMesh.materialIndex == createdMaterials.size()) {
            destMesh.material = createdMaterials.emplace_back(std::make_shared<Material>());
            destMesh.material->constantBuffer.CreateConstantBuffer(L"ToonModel ConstantBuffer", sizeof(MaterialData));
            MaterialData materialData{};
            materialData.diffuse = srcMaterial.diffuse;
            materialData.specular = srcMaterial.specular;
            destMesh.material->constantBuffer.Copy(materialData);
            // テクスチャ
            if (srcMaterial.textureIndex == createdTextures.size()) {
                destMesh.material->texture = createdTextures.emplace_back(std::make_shared<Texture>());
                if (srcTexture && !srcTexture->filePath.empty()) {
                    destMesh.material->texture->textureResource.CreateFromWICFile(commandContext, srcTexture->filePath.wstring());
                    destMesh.material->texture->sampler = SamplerManager::LinearWrap;
                }
                else {
                    // パスがない場合白画像を使う
                    destMesh.material->texture->textureResource = DefaultTexture::White;
                    destMesh.material->texture->sampler = SamplerManager::PointWrap;
                }
            }
            else {
                // 生成済みから引っ張ってくる
                destMesh.material->texture = createdTextures[srcMaterial.textureIndex];
            }
        }
        else {
            // 生成済みから引っ張ってくる
            destMesh.material = createdMaterials[srcMesh.materialIndex];
        }
    }
    commandContext.Close();
    auto& commandQueue = Graphics::GetInstance()->GetCommandQueue();
    commandQueue.Excute(commandContext);
    commandQueue.Signal();
    commandQueue.WaitForGPU();
}


std::list<ToonModelInstance*> ToonModelInstance::instanceLists_;

ToonModelInstance::ToonModelInstance() {
    instanceLists_.emplace_back(this);
}

ToonModelInstance::~ToonModelInstance() {
    auto iter = std::find(instanceLists_.begin(), instanceLists_.end(), this);
    if (iter != instanceLists_.end()) {
        instanceLists_.erase(iter);
    }
}
