#include "LightingPass.hlsli"

static const float PI = 3.1415926535f;

static float3 Position;
static float3 Normal;
static float3 ViewDirection;
static float3 Albedo;
static float Metallic;
static float Roughness;
static float Alpha;
static float AlphaSq;
static float NdotV;

struct PSInput {
    float4 svPosition : SV_POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

// 静的変数を初期化
void InitializeSurfaceProperties(PSInput input) {
     // 深度をサンプリング
    float depth = g_Depth.SampleLevel(g_Sampler, input.texcoord, 0);
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = input.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 tmpPosition = float4(xy, depth, 1.0f);
    tmpPosition = mul(tmpPosition, g_Scene.viewProjectionInverseMatrix);
    Position = tmpPosition.xyz / tmpPosition.w;
    Normal = g_Normal.SampleLevel(g_Sampler, input.texcoord, 0) * 2.0f - 1.0f;
    ViewDirection = normalize(g_Scene.cameraPosition  - Position);
    Albedo = g_Albedo.SampleLevel(g_Sampler, input.texcoord, 0);
    Metallic = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).x;
    Roughness = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).y;
    Alpha = Roughness * Roughness;
    AlphaSq = Alpha * Alpha;
    NdotV = saturate(dot(Normal, ViewDirection));
}

float Pow5(float n) {
    float n2 = n * n;
    return n2 * n2 * n;
}

float SchlickFresnel(float f0, float f90, float cosine) {
    return lerp(f0, f90, Pow5(1.0f - cosine));
}

float3 SchlickFresnel(float3 f0, float3 f90, float cosine) {
    return lerp(f0, f90, Pow5(1.0f - cosine));
}

float3 LambertDiffuse(float NdotL) {
    return Albedo * NdotL / PI;
}

float3 BurleyDiffuse(float NdotL, float LdotH) {
    float f90 = 0.5f + 2.0f * Roughness * LdotH * LdotH;
    return Albedo * SchlickFresnel(1.0f, f90, NdotL) * SchlickFresnel(1.0f, f90, NdotV);
}

float3 NormalizedDisneyDiffuse(float NdotL, float LdotH) {
    float energyBias = lerp(0.0f, 0.5f, Roughness);
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, Roughness);
    float f90 = energyBias + 2.0f * Roughness * LdotH * LdotH;
    float FL = SchlickFresnel(1.0f, f90, NdotL);
    float FV = SchlickFresnel(1.0f, f90, NdotV);
    return Albedo * FL * FV * energyFactor/* * (1.0f / PI)*/;
}

// GGX法線分布関数
float D_GGX(float NdotH) {
    float t = (NdotH * NdotH * (AlphaSq - 1.0f)) + 1.0f;
    return AlphaSq / (t * t * PI + 1e-6f);
}

//// GGXシャドウマスキング関数
//float G_Smith(float NdotL) {
//    float k = AlphaSq / 2.0f;
//    float G_Schlick_V = NdotV / (NdotV * (1.0f - k) + k);
//    float G_Schlick_L = NdotL / (NdotL * (1.0f - k) + k);
//    return G_Schlick_V + G_Schlick_L;
//}

// GGXシャドウマスキング関数
float G_GGX(float NdotL) {
    float Gv = NdotL * sqrt((-NdotV * AlphaSq + NdotV) * NdotV + AlphaSq);
    float Gl = NdotV * sqrt((-NdotL * AlphaSq + NdotL) * NdotL + AlphaSq);
    return 0.5f / (Gv + Gl + 1e-6f);
}

//float ShlickSmithHable(SurfaceProperties surface, LightProperties light) {
//    return 1.0f / lerp(light.LdotH * light.LdotH, 1.0f, surface.alphaSq * 0.25f);
//}

float3 SpecularBRDF(float NdotL, float LdotH, float NdotH) {
    
    static const float dielectricF0 = 0.04f;
    float3 f0 = lerp(dielectricF0, Albedo, Metallic);
    
    float3 F = SchlickFresnel(f0, 1.0f, LdotH);
    float D = D_GGX(NdotH);
    float V = G_GGX(NdotL);
    return D * F * V;
}

float3 BRDF(float3 lightDirection, float3 lightColor) {
    float3 halfVector = normalize(lightDirection + ViewDirection);
    float NdotL = saturate(dot(Normal, lightDirection));
    float LdotH = saturate(dot(lightDirection, halfVector));
    float NdotH = saturate(dot(Normal, halfVector));
    
    //float3 diffuse = NormalizedDisneyDiffuse(NdotL, LdotH);
    float3 diffuse = LambertDiffuse(NdotL);
    float3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
    //specular = 0.0f;
    return (diffuse * (1.0f - Metallic) + specular) * lightColor;
}

PSOutput main(PSInput input) {

    PSOutput output;
   
    InitializeSurfaceProperties(input);
    
    // 法線がないピクセルは使われていない
    clip(length(Normal) - 0.9f);
    
    
    float3 pixel = BRDF(-g_Scene.directionalLight.direction, g_Scene.directionalLight.color);
    
    output.color.rgb = pixel;
    output.color.a = 1.0f;
    
    return output;
}