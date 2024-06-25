#define TRUE_UINT 1
#define FALSE_UINT 0

#define INVALID_COLOR float32_t3(-1.0f, -1.0f, -1.0f)

#define USE_NORMAL_MAPS

//////////////////////////////////////////////////
// R + M * G + I HitGroupShaderRecordIndex      | R = RayContributionToHitGroupIndex
// 0 + 0 * G + 0 ShadowHitGroup                 | M = MultiplierForGeometryContributionToHitGroupIndex
// 1 + 2 * G + I PrimaryHitGroup                | G = GeometryContributionToHitGroupIndex
// 2 + 2 * G + I ReflectionHitGroup             | I = InstanceContributionToHitGroupIndex
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// 1 + 2 * G + I PrimaryHitGroup
#define PRIMARY_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 1
#define PRIMARY_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX 2
#define PRIMARY_MISS_SHADER_INDEX 0
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// 0 + 0 * G + 0 ShadowHitGroup
#define SHADOW_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 0
#define SHADOW_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX 0
#define SHADOW_MISS_SHADER_INDEX 1
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// 2 + 2 * G + I ReflectionHitGroup
#define REFLECTION_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 2
#define REFLECTION_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX 2
#define REFLECTION_MISS_SHADER_INDEX 2
//////////////////////////////////////////////////

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)

const uint32_t MAX_RECURSIVE_COUNT = 1;

// シーン
struct Scene {
    float32_t4x4 viewProjectionInverseMatrix;
    float32_t3 cameraPosition;
};

// ペイロード
struct Payload {
    float32_t3 color;
    uint32_t recursiveCount;    // 再帰回数
};

struct Attributes {
    float32_t2 barycentrics;
};

ConstantBuffer<Scene> g_Scene : register(b0);

RaytracingAccelerationStructure g_TLAS : register(t0);
TextureCube<float32_t4> g_Skybox : register(t1);
RWTexture2D<float32_t4> g_Color : register(u0);
SamplerState g_PointSampler : register(s0);
SamplerState g_LinearSampler : register(s1);

Texture2D<float32_t4> g_BindlessTextures[] : register(t0, space2);

//////////////////////////////////////////////////


// texcoodとdepthからワールド座標を計算
float32_t3 GetWorldPosition(in float32_t2 texcoord, in float32_t depth, in float32_t4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float32_t2 xy = texcoord * float32_t2(2.0f, -2.0f) + float32_t2(-1.0f, 1.0f);
    float32_t4 position = float32_t4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    position.xyz /= position.w;
    return position.xyz;
}

[shader("raygeneration")]
void RayGeneration() {
    // レイのインデックス
    uint32_t2 dispatchRaysIndex = DispatchRaysIndex().xy;
    // レイの縦横数
    uint32_t2 dispatchRaysDimensions = DispatchRaysDimensions().xy;
    // テクスチャ座標系を求める    
    float32_t2 texcoord = ((float32_t2) dispatchRaysIndex + 0.5f) / (float32_t2)dispatchRaysDimensions;

    //ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    // 近面から遠面へのレイ
    RayDesc ray;
    float32_t3 rayOrigin = GetWorldPosition(texcoord, 0.0f, g_Scene.viewProjectionInverseMatrix);
    ray.Origin = rayOrigin;
    float32_t3 rayDiff = GetWorldPosition(texcoord, 1.0f, g_Scene.viewProjectionInverseMatrix) - rayOrigin;
    ray.Direction = normalize(rayDiff);
    ray.TMin = 0.0f;
    ray.TMax = length(rayDiff);
    // レイを飛ばす
    Payload payload;
    payload.color = float32_t3(0.0f, 0.0f, 0.0f);
    payload.recursiveCount = 0; // 再帰回数 0
    TraceRay(
        g_TLAS, // RaytracingAccelerationStructure
        // 背面カリング
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        // レイのマスク
        PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
        // シェーダーテーブルのオフセット
        PRIMARY_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
        // シェーダーテーブルの係数
        PRIMARY_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // MultiplierForGeometryContributionToHitGroupIndex
        // ミスシェーダーのシェーダーテーブル
        PRIMARY_MISS_SHADER_INDEX, // MissShaderIndex
        ray, // Ray
        payload // Payload
    );

    g_Color[dispatchRaysIndex].rgb = payload.color;
    g_Color[dispatchRaysIndex].a = 1.0f;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

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
}

struct Material {
    float32_t3 albedo;
    float32_t metallic;
    float32_t roughness;
    uint32_t albedoMapIndex;
    uint32_t metallicRoughnessMapIndex;
    uint32_t nomralMapIndex;
};

StructuredBuffer<PackedVertex> l_VertexBuffer : register(t0, space1);
StructuredBuffer<uint32_t> l_IndexBuffer : register(t1, space1);
ConstantBuffer<Material> l_Material : register(b0, space1);

float32_t3 CalcBarycentrics(float32_t2 barycentrics) {
    return float32_t3(1.0f - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);
}

float32_t4 R10G10B10A2Tofloat32_t4(uint32_t value) {
    float32_t x = (float32_t)((value >> 0) & 0x3FF) / 1023.0f;
    float32_t y = (float32_t)((value >> 10) & 0x3FF) / 1023.0f;
    float32_t z = (float32_t)((value >> 20) & 0x3FF) / 1023.0f;
    float32_t w = (float32_t)((value >> 30) & 0x3) / 1023.0f;
    return float32_t4(x, y, z, w);
}
// 法線を計算
float32_t3 GetNormal(in float32_t3 normal, in float32_t3 tangent, in float32_t2 texcoord) {
    // 法線マップからサンプリング
    float32_t3 normalMap = g_BindlessTextures[g_Material.normalMapIndex].SampleLevel(g_PointSampler, texcoord, 0).xyz;
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

    float32_t3 normal = (float32_t3)0, tangent = (float32_t3)0;
    for (uint32_t i = 0; i < 3; ++i) {
        uint32_t index = l_IndexBuffer[primitiveID + i];
        vertex.position += l_VertexBuffer[index].position * barycentrics[i];
        normal += R10G10B10A2Tofloat32_t4(l_VertexBuffer[index].normal).xyz * barycentrics[i];
#ifdef USE_NORMAL_MAPS
        tanget += R10G10B10A2Tofloat32_t4(l_VertexBuffer[index].tangent).xyz * barycentrics[i];
#endif
        vertex.texcoord += l_VertexBuffer[index].texcoord * barycentrics[i];
    }
    
    // ワールド座標系に変換
    vertex.position = mul(float32_t4(vertex.position, 1.0f), ObjectToWorld4x3());
    // 正規化
    normal = mul(normalize(vertex.normal), (float32_t3x3) ObjectToWorld4x3());
#ifdef USE_NORMAL_MAPS
    tangent = mul(normalize(vertex.tangent), (float32_t3x3) ObjectToWorld4x3());
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
    if (payload.recursiveCount > MAX_RECURSIVE_COUNT) {
        payload.color = float32_t3(0.0f, 0.0f, 0.0f);
        return;
    }

    // レイの情報    
    float32_t hitT = RayTCurrent();
    float32_t3 rayOrigin = WorldRayOrigin();
    float32_t3 rayDirection = WorldRayDirection();
    // 頂点を取得
    Vertex vertex = GetVertex(attributes);

    // 入射光
    RayDesc incidentRay;
    incidentRay.Origin = vertex.position;
    incidentRay.Direction = normalize(reflect(rayDirection, vertex.normal));
    incidentRay.TMin = 0.001f;
    incidentRay.TMax = 100000.0f;

    // 再帰回数を増やす
    ++payload.recursiveCount;    

    TraceRay(
        g_TLAS, // RaytracingAccelerationStructure
        // 背面カリング
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        // レイのマスク
        PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
        // シェーダーテーブルのオフセット
        PRIMARY_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
        // シェーダーテーブルの係数
        PRIMARY_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // MultiplierForGeometryContributionToHitGroupIndex
        // ミスシェーダーのシェーダーテーブル
        PRIMARY_MISS_SHADER_INDEX, // MissShaderIndex
        incidentRay, // Ray
        payload // Payload
    );

// TODO:ここから下




    // 影
    bool reciveShadow = (InstanceID() == 1);
    if (reciveShadow) {
        // 衝突点からライトへのレイ
        RayDesc shadowRay;
        // 発射点を法線方向に少しずらす
        shadowRay.Origin = vertex.position;
        shadowRay.Direction = -g_Scene.sunLightDirection;
        shadowRay.TMin = 0.001f; // 少し浮かす
        shadowRay.TMax = 100000.0f; // 

        ShadowPayload shadowPayload;
        shadowPayload.isHit = 0;

        TraceRay(
            g_CastShadowTLAS,
            RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
            SHADOW_RAY_ATTRIBUTE,
            SHADOW_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
            SHADOW_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // MultiplierForGeometryContributionToHitGroupIndex
            SHADOW_MISS_SHADER_INDEX, // MissShaderIndex
            shadowRay,
            shadowPayload);

        payload.shadow = lerp(1.0f, 0.5f, shadowPayload.isHit);
    }

    if (l_Material.useLighting == 0) {
        return;
    }

    // 反射したレイの方向
    float32_t3 reflectionRayDirection = reflect(rayDirection, vertex.normal);
    RayDesc reflectionRay;
    reflectionRay.Origin = vertex.position;
    reflectionRay.Direction = normalize(reflectionRayDirection);
    reflectionRay.TMin = 0.0001f;
    reflectionRay.TMax = 10000.0f;

    ReflectionPayload reflectionPayload;
    TraceRay(
        g_TLAS, // RaytracingAccelerationStructure
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
        REFLECTION_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
        REFLECTION_MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // MultiplierForGeometryContributionToHitGroupIndex
        REFLECTION_MISS_SHADER_INDEX, // MissShaderIndex
        reflectionRay, // Ray
        reflectionPayload); // Payload

    float32_t3 specular = l_Material.specular;
    specular *= Lighting::PhongReflection(vertex.normal, WorldRayDirection(), g_Scene.sunLightDirection, l_Material.shininess);
    specular *= (reflectionPayload.isHit == 1) ? reflectionPayload.color : g_Scene.sunLightColor * g_Scene.sunLightIntensity;
    payload.reflection = specular;
    // payload.reflection = (reflectionPayload.isHit == 1) ? reflectionPayload.color : g_Scene.sunLightColor * g_Scene.sunLightIntensity;
}


//////////////////////////////////////////////////


[shader("miss")]
void PrimaryMiss(inout PrimaryPayload payload) {
    payload.shadow = 1.0f;
    payload.reflection = float32_t3(0.0f, 0.0f, 0.0f);
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////


[shader("closesthit")]
void ShadowClosestHit(inout ShadowPayload payload, in Attributes attributes) {
    payload.isHit = TRUE_UINT;
}


//////////////////////////////////////////////////


[shader("miss")]
void ShadowMiss(inout ShadowPayload payload) {
    payload.isHit = FALSE_UINT;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

// 反射レイ
[shader("closesthit")]
void ReflectionClosestHit(inout ReflectionPayload payload, in Attributes attributes) {
    payload.isHit = TRUE_UINT;
    // 頂点
    Vertex vertex = GetVertex(attributes);
    if (l_Material.useLighting == 1) {
        // 拡散反射
        float32_t3 diffuse = l_Material.diffuse;
        diffuse *= l_Material.color;
        diffuse *= l_Texture.SampleLevel(l_Sampler, vertex.texcoord, 0).rgb;
        diffuse *= Lighting::HalfLambertReflection(vertex.normal, g_Scene.sunLightDirection);
        // 鏡面反射
        float32_t3 specular = l_Material.specular;
        specular *= Lighting::PhongReflection(vertex.normal, WorldRayDirection(), g_Scene.sunLightDirection, l_Material.shininess);

        // シェーディングによる色
        payload.color = (diffuse + specular) * g_Scene.sunLightColor * g_Scene.sunLightIntensity;
    }
    else {
        payload.color = l_Texture.SampleLevel(l_Sampler, vertex.texcoord, 0).rgb * l_Material.color;
    }
}

//////////////////////////////////////////////////

[shader("miss")]
void ReflectionMiss(inout ReflectionPayload payload) {
    payload.isHit = FALSE_UINT;
    payload.color = float32_t3(0.0f, 0.0f, 0.0f);
}