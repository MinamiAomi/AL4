#include "Pathtracing.hlsli"
#include "../../PBR.hlsli"

// パックされた頂点
// NormalとTangentがR10G10B10A2
struct PackedVertex {
    float32_t3 position;
    uint32_t normal;
    uint32_t tangent;
    float32_t2 texcoord;
};

struct Vertex {
    float32_t3 position;
    float32_t3 normal;
    float32_t2 texcoord;
};

struct Material {
    float32_t3 albedo;
    float32_t metallic;
    float32_t roughness;
    uint32_t albedoMapIndex;
    uint32_t metallicRoughnessMapIndex;
    uint32_t normalMapIndex;
};

StructuredBuffer<PackedVertex> l_VertexBuffer : register(t0, space2);
StructuredBuffer<uint32_t> l_IndexBuffer : register(t1, space2);
ConstantBuffer<Material> l_Material : register(b0, space2);

float32_t3 CalcBarycentrics(float32_t2 barycentrics) {
    return float32_t3(1.0f - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);
}

float32_t4 R10G10B10A2Tofloat32_t4(uint32_t value) {
    float32_t x = (float32_t)((value >> 0) & 0x3FF) / 1023.0f;
    float32_t y = (float32_t)((value >> 10) & 0x3FF) / 1023.0f;
    float32_t z = (float32_t)((value >> 20) & 0x3FF) / 1023.0f;
    float32_t w = (float32_t)((value >> 30) & 0x3) / 1023.0f;
    return float32_t4(x, y, z, w) * 2.0f - 1.0f;
}
// 法線を計算
float32_t3 GetNormal(in float32_t3 normal, in float32_t3 tangent, in float32_t2 texcoord) {
    // 法線マップからサンプリング
    float32_t3 normalMap = g_BindlessTextures[l_Material.normalMapIndex].SampleLevel(g_PointSampler, texcoord, 0).xyz;
    // UNORMからSNORMに変換
    normalMap = normalMap * 2.0f - 1.0f;
    // NormalとTangentに垂直なベクトル
    float32_t3 binormal = normalize(cross(tangent, normal));
    // 新しい法線
    float32_t3 newNormal = normalMap.x * tangent + normalMap.y * binormal + normalMap.z * normal;
    return newNormal;
}
// ワールド座標の頂点を計算
Vertex GetVertex(Attributes attributes) {
    Vertex vertex = (Vertex)0;
    float32_t3 barycentrics = CalcBarycentrics(attributes.barycentrics);
    uint32_t primitiveID = PrimitiveIndex() * 3;

    float32_t3 normal = float32_t3(0.0f, 0.0f, 0.0f), tangent = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < 3; ++i) {
        uint32_t index = l_IndexBuffer[primitiveID + i];
        vertex.position += l_VertexBuffer[index].position * barycentrics[i];
        normal += R10G10B10A2Tofloat32_t4(l_VertexBuffer[index].normal).xyz * barycentrics[i];
#ifdef USE_NORMAL_MAPS
        tangent += R10G10B10A2Tofloat32_t4(l_VertexBuffer[index].tangent).xyz * barycentrics[i];
#endif
        vertex.texcoord += l_VertexBuffer[index].texcoord * barycentrics[i];
    }

    // ワールド座標系に変換
    vertex.position = mul(float32_t4(vertex.position, 1.0f), ObjectToWorld4x3());
    // 正規化
    normal = normalize(mul(normalize(normal), (float32_t3x3)ObjectToWorld4x3()));
#ifdef USE_NORMAL_MAPS
    tangent = normalize(mul(normalize(tangent), (float32_t3x3)ObjectToWorld4x3()));
    // 法線マップから引っ張ってくる
    vertex.normal = GetNormal(normal, tangent, vertex.texcoord);
#else
    vertex.normal = normal;
#endif

    return vertex;
}

[shader("closesthit")]
void RecursiveClosestHit(inout Payload payload, in Attributes attributes) {

    // 再帰回数が最大で光源に当たらなかった
    if (payload.recursiveCount >= MAX_RECURSIVE_COUNT) {
        payload.color = float32_t3(0.0f, 0.0f, 0.0f);
        return;
    }

    // レイの情報    
    float32_t hitT = RayTCurrent();
    float32_t3 rayOrigin = WorldRayOrigin();
    float32_t3 rayDirection = WorldRayDirection();
    // 頂点を取得
    Vertex vertex = GetVertex(attributes);
    payload.color = vertex.normal;

    float32_t3 incidentDirection = normalize(reflect(rayDirection, vertex.normal));
    // 入射方向のレイ
    RayDesc incidentRay;
    incidentRay.Origin = vertex.position;
    incidentRay.Direction = incidentDirection;
    incidentRay.TMin = 0.001f;
    incidentRay.TMax = 100000.0f;

    // 再帰回数を増やす
    ++payload.recursiveCount;

    TraceRay(
        g_TLAS, // RaytracingAccelerationStructure
        // 背面カリング
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        // レイのマスク
        RECURSIVE_MASK, // InstanceInclusionMask
        // シェーダーテーブルのオフセット
        RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
        // シェーダーテーブルの係数
        MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // MultiplierForGeometryContributionToHitGroupIndex
        // ミスシェーダーのシェーダーテーブル
        MISS_SHADER_INDEX, // MissShaderIndex
        incidentRay, // Ray
        payload // Payload
    );

    float32_t3 albedo = l_Material.albedo * g_BindlessTextures[l_Material.albedoMapIndex].SampleLevel(g_LinearSampler, vertex.texcoord, 0).rgb;
    float32_t2 metallicRoughness = float32_t2(l_Material.metallic, l_Material.roughness) * g_BindlessTextures[l_Material.metallicRoughnessMapIndex].SampleLevel(g_LinearSampler, vertex.texcoord, 0).zy;
    // 0が扱えないため
    metallicRoughness.y = clamp(metallicRoughness.y, 0.03f, 1.0f);
    PBR::Material material = PBR::CreateMaterial(albedo, metallicRoughness.x, metallicRoughness.y);
    PBR::Geometry geometry = PBR::CreateGeometry(vertex.position, vertex.normal, rayOrigin);
    PBR::IncidentLight incidentLight;
    incidentLight.direction = incidentDirection;
    incidentLight.color = payload.color * 3.14159265359f;

    float32_t3 irradiance = incidentLight.color * saturate(dot(geometry.normal, incidentLight.direction));
    float32_t3 brdf = PBR::SpecularBRDF(incidentLight.direction, geometry.normal, geometry.viewDirection, material.specularReflectance, material.specularRoughness);
    /*if (brdf.r >= 1.0f) {
        payload.color = float32_t3(1.0f, 0.0f, 0.0f);
        return;
    }*/
    payload.color = irradiance * brdf;

}