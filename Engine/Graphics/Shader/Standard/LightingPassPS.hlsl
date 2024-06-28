#include "LightingPass.hlsli"

static const float32_t PI = 3.14159265359f;
static const float32_t INV_PI = 0.31830988618f;
static const float32_t EPSILON = 0.00001f;

static float32_t3 Position;
static float32_t3 Normal;
static float32_t3 ViewDirection;
static float32_t3 Albedo;
static float32_t Metallic;
static float32_t Roughness;
static float32_t3 DiffuseReflectance;
static float32_t3 SpecularReflectance;
static float32_t Alpha;
static float32_t AlphaSq;
static float32_t NdotV;

// 変更

struct PSInput {
    float32_t4 svPosition : SV_POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float32_t4 color : SV_TARGET0;
};

// 静的変数を初期化
void InitializeSurfaceProperties(PSInput input) {
     // 深度をサンプリング
    float32_t depth = g_Depth.SampleLevel(g_DefaultSampler, input.texcoord, 0);
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float32_t2 xy = input.texcoord * float32_t2(2.0f, -2.0f) + float32_t2(-1.0f, 1.0f);
    float32_t4 tmpPosition = float32_t4(xy, depth, 1.0f);
    tmpPosition = mul(tmpPosition, g_Scene.viewProjectionInverseMatrix);
    Position = tmpPosition.xyz / tmpPosition.w;
    Normal = g_Normal.SampleLevel(g_DefaultSampler, input.texcoord, 0) * 2.0f - 1.0f;
    ViewDirection = normalize(g_Scene.cameraPosition - Position);
    Albedo = g_Albedo.SampleLevel(g_DefaultSampler, input.texcoord, 0).xyz;
    Metallic = g_MetallicRoughness.SampleLevel(g_DefaultSampler, input.texcoord, 0).x;
    Roughness = g_MetallicRoughness.SampleLevel(g_DefaultSampler, input.texcoord, 0).y;
    Roughness = max(Roughness, 0.02f);
    DiffuseReflectance = lerp(Albedo, 0.0f, Metallic);
    SpecularReflectance = lerp(0.04f, Albedo, Metallic);
    Alpha = Roughness;
    AlphaSq = Alpha * Alpha;
    NdotV = saturate(dot(Normal, ViewDirection));
}

float32_t Pow5(float32_t n) {
    float32_t n2 = n * n;
    return n2 * n2 * n;
}

//float32_t SchlickFresnel(float32_t f0, float32_t f90, float32_t cosine) {
//    return lerp(f0, f90, Pow5(1.0f - cosine));
//}

float32_t3 SchlickFresnel(float32_t3 f0, float32_t3 f90, float32_t cosine) {
    return lerp(f0, f90, Pow5(1.0f - cosine));
}

//float32_t3 BurleyDiffuse(float32_t NdotL, float32_t LdotH) {
//    float32_t f90 = 0.5f + 2.0f * Roughness * LdotH * LdotH;
//    return DiffuseReflectance * SchlickFresnel(1.0f, f90, NdotL) * SchlickFresnel(1.0f, f90, NdotV);
//}

//float32_t3 NormalizedDisneyDiffuse(float32_t NdotL, float32_t LdotH) {
//    float32_t energyBias = lerp(0.0f, 0.5f, Roughness);
//    float32_t energyFactor = lerp(1.0f, 1.0f / 1.51f, Roughness);
//    float32_t f90 = energyBias + 2.0f * Roughness * LdotH * LdotH;
//    float32_t FL = SchlickFresnel(1.0f, f90, NdotL);
//    float32_t FV = SchlickFresnel(1.0f, f90, NdotV);
//    return DiffuseReflectance * FL * FV * energyFactor * (1.0f / PI);
//}

// GGX法線分布関数
float32_t D_GGX(float32_t NdotH) {
    float32_t t = (NdotH * NdotH * (AlphaSq - 1.0f)) + 1.0f;
    return AlphaSq / (t * t * PI);
}

// GGXシャドウマスキング関数
float32_t G_Smith_Schlick_GGX(float32_t NdotL) {
    float32_t k = Alpha * 0.5f + EPSILON;
    float32_t GV = NdotV / (NdotV * (1.0f - k) + k);
    float32_t GL = NdotL / (NdotL * (1.0f - k) + k);
    return GV * GL;
}

// GGXシャドウマスキング関数
//float32_t G_GGX(float32_t NdotL) {
//    float32_t Gv = NdotL * sqrt((-NdotV * AlphaSq + NdotV) * NdotV + AlphaSq);
//    float32_t Gl = NdotV * sqrt((-NdotL * AlphaSq + NdotL) * NdotL + AlphaSq);
//    return 0.5f / (Gv + Gl + 1e-6f);
//}

float32_t3 F_Schlick(float32_t LdotH) {
    return (SpecularReflectance + (1.0f - SpecularReflectance) * Pow5(1.0f - LdotH));
}

float32_t3 DiffuseBRDF() {
    return DiffuseReflectance * INV_PI;
}

float32_t3 SpecularBRDF(float32_t NdotL, float32_t LdotH, float32_t NdotH) {
    float32_t3 F = F_Schlick(LdotH);
    float32_t D = D_GGX(NdotH);
    float32_t G = G_Smith_Schlick_GGX(NdotL);
    return (F * (D * G)) / (4.0f * NdotV * NdotL + EPSILON);
}

float32_t3 ShadeDirectionalLight(DirectionalLight light) {
    float32_t3 lightDirection = -light.direction;
    float32_t3 halfVector = normalize(lightDirection + ViewDirection);
    float32_t NdotL = saturate(dot(Normal, lightDirection));
    float32_t LdotH = saturate(dot(lightDirection, halfVector));
    float32_t NdotH = saturate(dot(Normal, halfVector));
        
    float32_t3 diffuse = DiffuseBRDF();
    float32_t3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
    float32_t3 BRDF = diffuse + specular;
    return BRDF * NdotL * light.color;
}

//float32_t GetDistanceAttenuation(float32_t3 unNormalizedLightVector) {
//    float32_t distSq = dot(unNormalizedLightVector, unNormalizedLightVector);
//    float32_t attenuation = 1.0f / max(distSq, EPSILON);
//    return attenuation;
//}
//
//float32_t3 ShadePointLight(PointLight light) {
//    float32_t3 diff = light.position - Position;
//    float32_t3 lightDirection = normalize(diff);
//    
//    float32_t3 halfVector = normalize(lightDirection + ViewDirection);
//    float32_t NdotL = saturate(dot(Normal, lightDirection));
//    float32_t LdotH = saturate(dot(lightDirection, halfVector));
//    float32_t NdotH = saturate(dot(Normal, halfVector));
//        
//    float32_t3 diffuse = DiffuseBRDF();
//    float32_t3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
//    float32_t3 BRDF = diffuse + specular;
//    
//    float32_t attenuation = GetDistanceAttenuation(diff);
//    
//    return BRDF * NdotL * light.color * attenuation * INV_PI;  
//}
//
//float32_t GetAngleAttenuation(float32_t3 unNormalizedLightVector, float32_t3 lightDirection, float32_t lightAngleScale, float32_t lightAngleOffset) {
//    float32_t cd = dot(lightDirection, unNormalizedLightVector);
//    float32_t attenuation = saturate(cd * lightAngleScale + lightAngleOffset);
//    attenuation *= attenuation;
//    return attenuation;
//}
//
//float32_t3 ShadeSpotLight(SpotLight light) {
//    float32_t3 diff = light.position - Position;
//    float32_t3 lightDirection = normalize(diff);
//    
//    float32_t3 halfVector = normalize(lightDirection + ViewDirection);
//    float32_t NdotL = saturate(dot(Normal, lightDirection));
//    float32_t LdotH = saturate(dot(lightDirection, halfVector));
//    float32_t NdotH = saturate(dot(Normal, halfVector));
//        
//    float32_t3 diffuse = DiffuseBRDF();
//    float32_t3 specular = SpecularBRDF(NdotL, LdotH, NdotH);
//    float32_t3 BRDF = diffuse + specular;
//    
//    float32_t attenuation = GetDistanceAttenuation(diff);
//    attenuation = GetAngleAttenuation(-lightDirection, light.direction, light.angleScale, light.angleOffset);
//    
//    return BRDF * NdotL * light.color * attenuation * INV_PI;
//}

float32_t3 DiffuseIBL() {
    return DiffuseReflectance * g_Irradiance.Sample(g_CubeMapSampler, Normal);
}

float32_t3 SpecularIBL() {
    float32_t lod = Roughness * (g_Scene.radianceMipCount);
    float32_t3 specular = SchlickFresnel(SpecularReflectance, 1.0f, NdotV);
    return specular * g_Radiance.SampleLevel(g_CubeMapSampler, reflect(-ViewDirection, Normal), lod);
}

PSOutput main(PSInput input) {

    PSOutput output;
   
    InitializeSurfaceProperties(input);
    
    // AlbedoのWが0の場合は計算しない
    if (g_Albedo.SampleLevel(g_DefaultSampler, input.texcoord, 0).w == 0.0f) {
        discard;
    }
    
    float32_t3 color = 0.0f;
    //color += ShadeDirectionalLight(g_Scene.directionalLight);
    //color += DiffuseIBL();
    color += SpecularIBL();

    output.color.rgb = color;
    output.color.a = 1.0f;
    
    return output;
}