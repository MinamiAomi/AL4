#include "LightingPass.hlsli"

static const float PI = 3.1415926535f;

struct SurfaceProperties {
    float position;
    float3 normal;
    float3 viewDirection;
    float3 albedo;
    float metallic;
    float roughness;
    float alpha;
    float alphaSq;
    float NdotV;
};

struct LightProperties {
    float3 direction;
    float3 color;
    float NdotL;
    float LdotH;
    float NdotH;
    
    void Initialize(SurfaceProperties SurfaceProperties, DirectionalLight light) {
        direction = light.direction;
        color = light.color;
        float3 halfVector = normalize(light.direction + SurfaceProperties.viewDirection);
        NdotL = saturate(dot(SurfaceProperties.normal, light.direction));
        LdotH = saturate(dot(light.direction, halfVector));
        NdotH = saturate(dot(SurfaceProperties.normal, halfVector));
    }
};

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

//float3 BurleyDiffuse(SurfaceProperties surface, LightProperties light) {
//    float f90 = 0.5f + 2.0f * surface.roughness * light.LdotH * light.LdotH;
//    return surface.albedo * SchlickFresnel(1.0f, f90, light.NdotL) * SchlickFresnel(1.0f, f90, surface.NdotV);
//}

float NormalizedDisneyDiffuse(SurfaceProperties surface, LightProperties light) {
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, surface.roughness);
    float f90 = 0.5f * surface.roughness + 2.0f * surface.roughness * light.NdotL * light.NdotL;
    float FL = SchlickFresnel(1.0f, f90, light.NdotL);
    float FV = SchlickFresnel(1.0f, f90, surface.NdotV);
    return FL * FV * energyFactor;
}

// GGX法線分布関数
float D_GGX(SurfaceProperties surface, LightProperties light) {
    float t = (light.NdotH * light.NdotH * (surface.alphaSq - 1.0f)) + 1.0f;
    return surface.alphaSq / t * t * PI;
}

// GGXシャドウマスキング関数
float G_GGX(SurfaceProperties surface, LightProperties light) {
    float Gv = light.NdotL * sqrt(surface.NdotV * surface.NdotV * (1.0f - surface.alphaSq) + surface.alphaSq);
    float Gl = surface.NdotV * sqrt(light.NdotL * light.NdotL * (1.0f - surface.alphaSq) + surface.alphaSq);
    return 0.5f / (Gv + Gl);
}

//float ShlickSmithHable(SurfaceProperties surface, LightProperties light) {
//    return 1.0f / lerp(light.LdotH * light.LdotH, 1.0f, surface.alphaSq * 0.25f);
//}

float3 SpecularBRDF(SurfaceProperties surface, LightProperties light) {
    
    static const float dielectricF0 = 0.04f;
    float f0 = lerp(dielectricF0, surface.albedo, surface.metallic);
    
    float3 F = SchlickFresnel(f0, 1.0f, light.LdotH);
    float D = D_GGX(surface, light);
    float V = G_GGX(surface, light);
    return D * F * V;
}

float3 BRDF(SurfaceProperties surface, LightProperties light) {
    float3 diffuse = NormalizedDisneyDiffuse(surface, light);
    float3 specular = SpecularBRDF(surface, light);
    return (diffuse * (1.0f - surface.metallic) + specular) * light.color;
}

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

PSOutput main(PSInput input) {

    PSOutput output;
   
    SurfaceProperties surface;
    surface.position = GetPosition(input.texcoord);
    surface.normal = g_Normal.SampleLevel(g_Sampler, input.texcoord, 0) * 2.0f - 1.0f;
    surface.viewDirection = normalize(g_Scene.cameraPosition - surface.position);
    surface.albedo = g_Albedo.SampleLevel(g_Sampler, input.texcoord, 0);
    surface.metallic = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).x;
    surface.roughness = g_MetallicRoughness.SampleLevel(g_Sampler, input.texcoord, 0).y;
    surface.alpha = surface.roughness * surface.roughness;
    surface.alphaSq = surface.alpha * surface.alpha;
    surface.NdotV = saturate(dot(surface.normal, surface.viewDirection));
    
    LightProperties light;
    light.Initialize(surface, g_Scene.directionalLight);

    float3 pixel = BRDF(surface, light);
    
    output.color.rgb = pixel;
    output.color.a = 1.0f;
    
    return output;
}