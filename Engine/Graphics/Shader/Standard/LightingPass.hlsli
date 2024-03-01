struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 cameraPosition;
    uint numDirectionalLights;
    uint numPointLights;
    uint numSpotLights;
};

struct DirectionalLight {
    float3 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<Scene> g_Scene : register(b0);

// G-Buffers
Texture2D<float3> g_Albedo : register(t0);
Texture2D<float2> g_MetallicRoughness : register(t1);
Texture2D<float3> g_Normal : register(t2);
Texture2D<float> g_Depth : register(t3);

// Lights
StructuredBuffer<DirectionalLight> g_DirectionalLights : register(t4);

SamplerState g_Sampler : register(s0);

