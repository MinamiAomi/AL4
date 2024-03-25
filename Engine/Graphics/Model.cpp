#include "Model.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/CommandContext.h"
#include "Core/TextureLoader.h"
#include "Mesh.h"
#include "Material.h"

namespace {

    uint32_t R32G32B32ToR10G10B10A2(const Vector3& in) {
        uint32_t x = static_cast<uint32_t>(std::clamp((in.x + 1.0f) * 0.5f, 0.0f, 1.0f) * 0x3FF) & 0x3FF;
        uint32_t y = static_cast<uint32_t>(std::clamp((in.y + 1.0f) * 0.5f, 0.0f, 1.0f) * 0x3FF) & 0x3FF;
        uint32_t z = static_cast<uint32_t>(std::clamp((in.z + 1.0f) * 0.5f, 0.0f, 1.0f) * 0x3FF) & 0x3FF;
        return x | y << 10 | z << 20;
    }

    std::vector<Mesh> ParseMeshes(const aiScene* scene, const std::vector<std::shared_ptr<PBRMaterial>>& materials) {
        std::vector<Mesh> meshes(scene->mNumMeshes);

        for (uint32_t meshIndex = 0; auto & destMesh : meshes) {
            const aiMesh* srcMesh = scene->mMeshes[meshIndex];
            assert(srcMesh->HasNormals() && srcMesh->HasTangentsAndBitangents());

            destMesh.vertices.resize(srcMesh->mNumVertices);
            for (uint32_t vertexIndex = 0; auto & destVertex : destMesh.vertices) {
                aiVector3D& srcPosition = srcMesh->mVertices[vertexIndex];
                aiVector3D& srcNormal = srcMesh->mNormals[vertexIndex];
                aiVector3D& srcTangent = srcMesh->mTangents[vertexIndex];
                // セット
                destVertex.position = { srcPosition.x, srcPosition.y, srcPosition.z };
                Vector3 tmpNormal = { srcNormal.x, srcNormal.y, srcNormal.z };
                Vector3 tmpTangent = { srcTangent.x, srcTangent.y, srcTangent.z };
                if (srcMesh->HasTextureCoords(0)) {
                    aiVector3D& srcTexcoord = srcMesh->mTextureCoords[0][vertexIndex];
                    destVertex.texcood = { srcTexcoord.x, srcTexcoord.y };
                }
                else {
                    destVertex.texcood = Vector2::zero;
                }
                // 左手座標系に変換
                destVertex.position.z *= -1.0f;
                tmpNormal.z *= -1.0f;
                tmpTangent.z *= -1.0f;

                destVertex.normal = R32G32B32ToR10G10B10A2(tmpNormal);
                destVertex.tangent = R32G32B32ToR10G10B10A2(tmpTangent);

                vertexIndex++;
            }

            destMesh.indices.reserve(srcMesh->mNumFaces * 3);
            for (uint32_t faceIndex = 0; faceIndex < srcMesh->mNumFaces; ++faceIndex) {
                aiFace& srcFace = srcMesh->mFaces[faceIndex];
                assert(srcFace.mNumIndices == 3);
                destMesh.indices.emplace_back(srcFace.mIndices[0]);
                destMesh.indices.emplace_back(srcFace.mIndices[2]);
                destMesh.indices.emplace_back(srcFace.mIndices[1]);
            }

            // マテリアルが読み込まれてない
            assert(srcMesh->mMaterialIndex < materials.size());
            destMesh.material = materials[srcMesh->mMaterialIndex];

            ++meshIndex;
        }
        return meshes;
    }

    std::vector<std::shared_ptr<Material>> ParseMaterials(const aiScene* scene, const std::filesystem::path& directory) {
        std::vector<std::shared_ptr<Material>> materials(scene->mNumMaterials);

        for (uint32_t materialIndex = 0; auto & destMaterial : materials) {
            const aiMaterial* srcMaterial = scene->mMaterials[materialIndex];
            destMaterial = std::make_shared<Material>();

            aiColor3D diffuse{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == aiReturn_SUCCESS) {
                destMaterial->diffuse = { diffuse.r, diffuse.g, diffuse.b };
            }
            aiColor3D specular{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular) == aiReturn_SUCCESS) {
                destMaterial->specular = { specular.r, specular.g, specular.b };
            }
            aiColor3D ambient{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == aiReturn_SUCCESS) {
                destMaterial->ambient = { ambient.r, ambient.g, ambient.b };
            }
            float shininess{};
            if (srcMaterial->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
                destMaterial->shininess = shininess;
            }

            // テクスチャが一つ以上ある
            if (srcMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString path;
                srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
                // 読み込む
                // TextureLoader内で多重読み込み対応済み
                std::string filename(path.C_Str());
                destMaterial->diffuseMap = TextureLoader::Load(directory / filename);
            }
            ++materialIndex;
        }
        return materials;
    }
    std::vector<std::shared_ptr<PBRMaterial>> ParsePBRMaterials(const aiScene* scene, const std::filesystem::path& directory) {
        std::vector<std::shared_ptr<PBRMaterial>> materials(scene->mNumMaterials);

        for (uint32_t materialIndex = 0; auto & destMaterial : materials) {
            const aiMaterial* srcMaterial = scene->mMaterials[materialIndex];
            destMaterial = std::make_shared<PBRMaterial>();

            aiColor3D albedo{};
            if (srcMaterial->Get(AI_MATKEY_BASE_COLOR, albedo) == aiReturn_SUCCESS) {
                destMaterial->albedo = { albedo.r, albedo.g, albedo.b };
            }
            float metallic{};
            if (srcMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == aiReturn_SUCCESS) {
                destMaterial->metallic = metallic;
            }
            float roughness{};
            if (srcMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == aiReturn_SUCCESS) {
                destMaterial->roughness = roughness;
            }

            // テクスチャが一つ以上ある
            if (srcMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
                aiString path;
                srcMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
                // 読み込む
                // TextureLoader内で多重読み込み対応済み
                std::string filename(path.C_Str());
                destMaterial->albedoMap = TextureLoader::Load(directory / filename);
            }
            // テクスチャが一つ以上ある
            if (srcMaterial->GetTextureCount(aiTextureType_METALNESS) > 0 &&
                srcMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
                aiString metallicPath;
                aiString roughnessPath;
                srcMaterial->GetTexture(aiTextureType_METALNESS, 0, &metallicPath);
                srcMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessPath);
                // 同じテクスチャの場合使用
                if (metallicPath == roughnessPath) {
                    // 読み込む
                    // TextureLoader内で多重読み込み対応済み
                    std::string filename(metallicPath.C_Str());
                    destMaterial->metallicRoughnessMap = TextureLoader::Load(directory / filename);
                }
            }
            // テクスチャが一つ以上ある
            if (srcMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
                aiString path;
                srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &path);
                // 読み込む
                // TextureLoader内で多重読み込み対応済み
                std::string filename(path.C_Str());
                destMaterial->normalMap = TextureLoader::Load(directory / filename);
            }
            ++materialIndex;
        }
        return materials;
    }

}

std::list<ModelInstance*> ModelInstance::instanceLists_;

std::shared_ptr<Model> Model::Load(const std::filesystem::path& path) {

    // privateコンストラクタをmake_sharedで呼ぶためのヘルパー
    struct Helper : Model {
        Helper() : Model() {}
    };
    std::shared_ptr<Model> model = std::make_shared<Helper>();

    auto directory = path.parent_path();

    Assimp::Importer importer;
    int flags = 0;
    // 三角形のみ
    flags |= aiProcess_Triangulate;
    // 左手座標系に変換
    flags |= aiProcess_FlipUVs;
    // 接空間を計算
    flags |= aiProcess_GenNormals;
    flags |= aiProcess_CalcTangentSpace;
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    // 読み込めた
    if (!scene) {
        OutputDebugStringA(importer.GetErrorString());
        assert(false);
    }
    assert(scene->HasMeshes());

    auto materials = ParsePBRMaterials(scene, directory);
    model->meshes_ = ParseMeshes(scene, materials);

    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    // 中間リソースをコピーする
    for (auto& mesh : model->meshes_) {
        mesh.CreateBuffers(commandContext);
    }
    model->blas_.Create(L"ModelBLAS", commandContext, model->meshes_);
    commandContext.Finish(true);


    return model;
}

ModelInstance::ModelInstance() {
    instanceLists_.emplace_back(this);
}

ModelInstance::~ModelInstance() {
    std::erase(instanceLists_, this);
    // auto iter = std::find(instanceLists_.begin(), instanceLists_.end(), this);
    // if (iter != instanceLists_.end()) {
    //     instanceLists_.erase(iter);
    // }
}
