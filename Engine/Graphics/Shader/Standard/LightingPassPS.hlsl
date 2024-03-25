#include "LightingPass.hlsli"

static const float PI = 3.14159265359f;
static const float INV_PI = 0.31830988618f;
static const float EPSILON = 0.00001f;

static float3 Position;
static float3 Normal;
static float3 ViewDirection;
static float3 Albedo;
static float Metallic;
static float Roughness;
static float3 DiffuseReflectance;
static float3 SpecularReflectance;
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
    ViewDirection = normalize(g_Scene.cameraPosition - Position);
    Albedo = g_Albedo.SampleLevel(g_Sampler, input.texcoord, 0).xyz;
    Metallic = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).x;
    Roughness = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).y;
    Roughness = max(Roughness, 0.02f);
    DiffuseReflectance = lerp(Albedo, 0.0f, Metallic);
    SpecularReflectance = lerp(0.04f, Albedo, Metallic);
    Alpha = Roughness;
    AlphaSq = Alpha * Alpha;
    NdotV = saturate(dot(Normal, ViewDirection));
}

float Pow5(float n) {
    float n2 = n * n;
    return n2 * n2 * n;
}

//float SchlickFresnel(float f0, float f90, float cosine) {
//    return lerp(f0, f90, Pow5(1.0f - cosine));
//}

//float3 SchlickFresnel(float3 f0, float3 f90, float cosine) {
//    return lerp(f0, f90, Pow5(1.0f - cosine));
//}

//float3 BurleyDiffuse(float NdotL, float LdotH) {
//    float f90 = 0.5f + 2.0f * Roughness * LdotH * LdotH;
//    return DiffuseReflectance * SchlickFresnel(1.0f, f90, NdotL) * SchlickFresnel(1.0f, f90, NdotV);
//}

//float3 NormalizedDisneyDiffuse(float NdotL, float LdotH) {
//    float energyBias = lerp(0.0f, 0.5f, Roughness);
//    float energyFactor = lerp(1.0f, 1.0f / 1.51f, Roughness);
//    float f90 = energyBias + 2.0f * Roughness * LdotH * LdotH;
//    float FL = SchlickFresnel(1.0f, f90, NdotL);
//    float FV = SchlickFresnel(1.0f, f90, NdotV);
//    return DiffuseReflectance * FL * FV * energyFactor * (1.0f / PI);
//}

// GGX法線分布関数
float D_GGX(float NdotH) {
    float t = (NdotH * NdotH * (AlphaSq - 1.0f)) + 1.0f;
    return AlphaSq / (t * t * PI);
}

// GGXシャドウマスキング関数
float G_Smith_Schlick_GGX(float NdotL) {
    float k = Alpha * 0.5f + EPSILON;
    float GV = NdotV / (NdotV * (1.0f - k) + k);
    float GL = NdotL / (NdotL * (1.0f - k) + k);
    return GV * GL;
}

// GGXシャドウマスキング関数
//float G_GGX(float NdotL) {
//    float Gv = NdotL * sqrt((-NdotV * AlphaSq + NdotV) * NdotV + AlphaSq);
//    float Gl = NdotV * sqrt((-NdotL * AlphaSq + NdotL) * NdotL + AlphaSq);
//    return 0.5f / (Gv + Gl + 1e-6f);
//}

float3 F_Schlick(float LdotH) {
    return (SpecularReflectance + (1.0f - SpecularReflectance) * Pow5(1.0f - LdotH));
}

float3 DiffuseBRDF() {
    return DiffuseReflectance * INV_PI;
}

float3 SpecularBRDF(float NdotL, float LdotH, float NdotH) {
    float3 F = F_Schlick(LdotH);
    float D = D_GGX(NdotH);
    float G = G_Smith_Schlick_GGX(NdotL);
    return (F * (D * G)) / (4.0f * NdotV * NdotL + EPSILON);
}

float3 ShadeDirectionalLight(DirectionalLight light) {
    float3 lightDirection = -light.direction;
    float3 halfVector = normalize(lightDirection + ViewDirection);
    float NdotL = saturate(dot(Normal, lightDirection));
    float LdotH = saturate(dot(lightDirection, halfVector));
    float NdotH = saturate(dot(Normal, halfVector));
        
    float3 diffuse = DiffuseBRDF();
    float3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
    float3 BRDF = diffuse + specular;
    return BRDF * NdotL * light.color;
}

float GetDistanceAttenuation(float3 unNormalizedLightVector) {
    float distSq = dot(unNormalizedLightVector, unNormalizedLightVector);
    float attenuation = 1.0f / max(distSq, EPSILON);
    return attenuation;
}

float3 ShadePointLight(PointLight light) {
    float3 diff = light.position - Position;
    float3 lightDirection = normalize(diff);
    
    float3 halfVector = normalize(lightDirection + ViewDirection);
    float NdotL = saturate(dot(Normal, lightDirection));
    float LdotH = saturate(dot(lightDirection, halfVector));
    float NdotH = saturate(dot(Normal, halfVector));
        
    float3 diffuse = DiffuseBRDF();
    float3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
    float3 BRDF = diffuse + specular;
    
    float attenuation = GetDistanceAttenuation(diff);
    
    return BRDF * NdotL * light.color * attenuation * INV_PI;  
}

float GetAngleAttenuation(float3 unNormalizedLightVector, float3 lightDirection, float lightAngleScale, float lightAngleOffset) {
    float cd = dot(lightDirection, unNormalizedLightVector);
    float attenuation = saturate(cd * lightAngleScale + lightAngleOffset);
    attenuation *= attenuation;
    return attenuation;
}

float3 ShadeSpotLight(SpotLight light) {
    float3 diff = light.position - Position;
    float3 lightDirection = normalize(diff);
    
    float3 halfVector = normalize(lightDirection + ViewDirection);
    float NdotL = saturate(dot(Normal, lightDirection));
    float LdotH = saturate(dot(lightDirection, halfVector));
    float NdotH = saturate(dot(Normal, halfVector));
        
    float3 diffuse = DiffuseBRDF();
    float3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
    float3 BRDF = diffuse + specular;
    
    float attenuation = GetDistanceAttenuation(diff);
    attenuation = GetAngleAttenuation(-lightDirection, light.direction, light.angleScale, light.angleOffset);
    
    return BRDF * NdotL * light.color * attenuation * INV_PI;
}

PSOutput main(PSInput input) {

    PSOutput output;
   
    InitializeSurfaceProperties(input);
    
    // AlbedoのWが0の場合は計算しない
    if (g_Albedo.SampleLevel(g_Sampler, input.texcoord, 0).w == 0.0f) {
        discard;
    }
    
    float3 color = 0.0f;
    color += ShadeDirectionalLight(g_Scene.directionalLight);
    
    output.color.rgb = color;
    output.color.a = 1.0f;
    
    return output;
}