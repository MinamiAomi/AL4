#include "Math.hlsli"

cbuffer Sceme : register(b0) {
    float4x4 viewProjectionInverse_;
}

RWTexture2D<float4> output_ : register(u0);

#define MAX_STEPS 16
#define MAX_DISTANCE 100
#define EPSILON 0.01f

// 距離関数
float GetDistance(in float3 position) {
    float3 spherePosition = float3(0.0f, 0.0f, 0.0f);
    float sphereRadius = 1.0f;
    
    float distance = length(position - spherePosition) - sphereRadius;
    
    return distance;
}

float3 GetNormal(in float3 position) {
    const float2 epsilon = float2(0.01f, 0.0f);
    
    float distance = GetDistance(position);
    float3 normal = distance -
        float3(GetDistance(position - epsilon.xyy),
               GetDistance(position - epsilon.yxy),
               GetDistance(position - epsilon.yyx));
    return normalize(normal);
}

float RayMarch(in float3 rayOrigin, in float3 rayDirection) {
    
    float totalDepth;
    
    for (uint i = 0; i < MAX_STEPS; ++i) {
        float3 p = rayOrigin + rayDirection * totalDepth;
        float depth = GetDistance(p);
        totalDepth += depth;
        
        if (totalDepth > MAX_DISTANCE || depth < EPSILON) {
            break;
        }
    }
    
    return totalDepth;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
      
    // 出力画像の画素数
    float2 dimensions;
    output_.GetDimensions(dimensions.x, dimensions.y);
    // 正規化デバイス座標系を取得
    float3 ndc = float3(LaunchIndexToNDC(float2(DTid.xy), dimensions), 0.0f);
    // 近クリップ面のワールド座標を取得
    float3 nearPosition = HomogeneousCoordinateTransform(ndc, viewProjectionInverse_);
    
    // 原点
    float3 rayOrigin = viewProjectionInverse_[3].xyz;
    // 方向
    float3 rayDirection = normalize(nearPosition - rayOrigin);
    // 距離
    float distance = RayMarch(rayOrigin, rayDirection);
    
    // 座標
    float3 position = rayOrigin + rayDirection * distance;
    // 法線
    float3 normal = GetNormal(position);
    
    float4 color;

    output_[DTid.xy] = color;
}