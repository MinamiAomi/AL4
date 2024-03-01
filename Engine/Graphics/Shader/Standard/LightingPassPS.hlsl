#include "LightingPass.hlsli"

float SchlickFresnel(float f0, float f90, float cosine) {
    
}

struct Pixel {
    float position;
    float3 normal;
    float3 viewDirection;
    float3 albedo;
    float metallic;
    float roughness;
    
    
    float3 BRDF(in float3 lightDirection) {
        float3 halfLightView = normalize(lightDirection + viewDirection);
        
        float energyBias = 0.5f * roughness;
        float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
        
        float dotLH = saturate(dot(lightDirection, halfLightView));
        float Fd90 = energyBias + 2.0f * dotLH * dotLH * roughness;
        float dotNL = saturate(dot(normal, lightDirection));
        float dotNV = saturate(dot(normal, viewDirection));
        
        
    }
    
};

struct PSInput {
    float4 svPosition : SV_POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

// texcoodとdepthからワールド座標を計算
float3 GetPosition(in float2 texcoord) {
    // 深度をサンプリング
    float depth = g_Depth.SampleLevel(g_Sampler, texcoord, 0);
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, g_Scene.viewProjectionInverseMatrix);
    return position.xyz / position.w;
}

float CalcDiffuseFromFresnel(in float3 N, in float3 L, in float3 V, in float3 H) {
    
    float energyBias = 0.5f * 
    
    
}

PSOutput main(PSInput input) {

    PSOutput output;
   
    Pixel pixel;
    pixel.position = GetPosition(input.texcoord);
    pixel.normal = g_Normal.SampleLevel(g_Sampler, input.texcoord, 0) * 2.0f - 1.0f;
    pixel.viewDirection = normalize(g_Scene.cameraPosition - pixel.position);
    pixel.albedo = g_Albedo.SampleLevel(g_Sampler, input.texcoord, 0);
    pixel.metallic = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).x;
    pixel.roughness = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).y;
        
    
    
    
    
    
    // 計算する
    output.color.xyz = baseColor * saturate(dot(normal, -directionalLights[0].direction));
    output.color.a = 1.0f;
    
    return output;
}