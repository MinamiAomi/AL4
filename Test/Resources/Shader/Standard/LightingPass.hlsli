struct DirectionalLight {
    float3 color;
    float intensity;
    float3 direction;
};

struct PointLight{
    float3 color;
    float intensity;
    float3 position;
};

struct SpotLight {
    float3 color;
    float intensity;
    float3 position;
    float angleScale;
    float3 direction;
    float angleOffset;
};

struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 cameraPosition;
    float pad;
    DirectionalLight directionalLight;
};

ConstantBuffer<Scene> g_Scene : register(b0);

// G-Buffers
Texture2D<float4> g_Albedo : register(t0);
Texture2D<float2> g_MetallicRoughness : register(t1);
Texture2D<float3> g_Normal : register(t2);
Texture2D<float> g_Depth : register(t3);

SamplerState g_Sampler : register(s0);
