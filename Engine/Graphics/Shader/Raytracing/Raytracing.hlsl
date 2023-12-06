#define GLOBAL_REGISTER_SPACE space0
#define HIT_GROUP_REGISTER_SPACE space1

struct DescriptorIndex {
    uint tlas;
    uint output;
    uint scene;
    uint light;
};

ConstantBuffer<DescriptorIndex> descriptorIndex : register(b0, GLOBAL_REGISTER_SPACE);

struct Scene {
    float4x4 viewProjectionInverseMatrix;
};

struct Light {
    float3 sunLightDirection;
};


// 一次レイ用ペイロード
struct PrimaryPayload {
    uint shadow;
};

#define TRUE_UINT 1
#define FALSE_UINT 0

// 一次レイヒットグループ
#define PRIMARY_HIT_GROUP_INDEX 0
// 影ヒットグループ
#define SHADOW_HIT_GROUP_INDEX 1

#define MISS_SHADER_INDEX 0

#define RECIVE_SHADOW_INSTANCE_ID 0x1
#define CAST_SHADOW_INSTANCE_ID 0x2

// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    return position.xyz / position.w;
}

[shader("raygeneration")]
void RayGeneration() {
    // レイのインデックス
    uint2 dispatchRaysIndex = DispatchRaysIndex().xy;
    // レイの縦横数
    uint2 dispatchRaysDimensions = DispatchRaysDimensions().xy;
    // テクスチャ座標系を求める    
    float2 texcoord = (float2) dispatchRaysIndex / (float2) dispatchRaysDimensions;
    
    ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    // 近面から遠面へのレイ
    RayDesc rayDesc;
    rayDesc.Origin = GetWorldPosition(texcoord, 0.0f, scene.viewProjectionInverseMatrix);
    float3 rayDiff = GetWorldPosition(texcoord, 0.0f, scene.viewProjectionInverseMatrix) - rayDesc.Origin;
    rayDesc.Direction = normalize(rayDiff);
    rayDesc.TMin = 0.0f;
    rayDesc.TMax = length(rayDiff);
    // レイを飛ばす
    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[descriptorIndex.tlas];
    PrimaryPayload payload;
    TraceRay(
        tlas,
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
        0xFF,
        PRIMARY_HIT_GROUP_INDEX,
        0,
        MISS_SHADER_INDEX,
        rayDesc,
        payload);

    float shadow = lerp(0.0f, 0.5f, payload.shadow);
    RWTexture2D<float> output = ResourceDescriptorHeap[descriptorIndex.output];
    output[dispatchRaysIndex] = shadow;
}

[shader("miss")]
void Miss(inout PrimaryPayload payload) {
    payload.shadow = FALSE_UINT;
}

struct Material {
    uint reciveShadow;
};
ConstantBuffer<Material> material : register(b0, HIT_GROUP_REGISTER_SPACE);

[shader("closesthit")]
void PrimaryRayClosestHit(inout PrimaryPayload payload, in BuiltInTriangleIntersectionAttributes attribs) {
    
    if ((InstanceID() & RECIVE_SHADOW_INSTANCE_ID) == 1) {
        // 影を受けるためシャドウレイを飛ばす
        float hitT = RayTCurrent();
        float3 rayOrigin = WorldRayOrigin();
        float3 rayDirection = WorldRayDirection();
        // ヒットしたポジション
        float3 hitPosition = rayOrigin + hitT * rayDirection;
    
        ConstantBuffer<Light> light = ResourceDescriptorHeap[descriptorIndex.light];
        // 衝突点からライトへのレイ
        RayDesc rayDesc;
        rayDesc.Origin = hitPosition;
        rayDesc.Direction = -light.sunLightDirection;
        rayDesc.TMin = 0.001f; // 少し浮かす
        rayDesc.TMax = 100000.0f; // 
    
        RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[descriptorIndex.tlas];
        TraceRay(
        tlas,
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
        ~1,
        SHADOW_HIT_GROUP_INDEX,
        0,
        MISS_SHADER_INDEX,
        rayDesc,
        payload);
    }
    else {
        // 影を受けない
        payload.shadow = FALSE_UINT;
    }
}

[shader("closesthit")]
void ShadowRayClosestHit(inout PrimaryPayload payload, in BuiltInTriangleIntersectionAttributes attribs) {   
    payload.shadow = TRUE_UINT;
}