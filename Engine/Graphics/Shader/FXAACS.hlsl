RWTexture2D<float4> g_Texture : register(u0);
SamplerState g_Sampler : register(s0);

uint2 AdjustDimensions(float2 uv, float2 dimensions, float2 offset) {
    return uint2(clamp(uv + offset, 0, dimensions) * dimensions);
}

float3 Sample(float2 center, float2 dimensions, float2 offset) {
    float2 uv = center + offset;
    uv = clamp(uv, 0.0f, dimensions - 1.0f);
    uv *= dimensions;
    float3 color = g_Texture[uint2(uv)].xyz;
    return color;
}

float RGBToLuminance(float3 rgb) {
    return dot(rgb, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    float2 dimensions;
    g_Texture.GetDimensions(dimensions.x, dimensions.y);
    float2 center = DTid.xy / dimensions;
    float2 offset = 1.0f / dimensions;
    
    float m = RGBToLuminance(Sample(center, dimensions, float2(0.0f, 0.0f)));
    float n = RGBToLuminance(Sample(center, dimensions, float2(0.0f, offset.y)));
    float e = RGBToLuminance(Sample(center, dimensions, float2(offset.x, 0.0f)));
    float s = RGBToLuminance(Sample(center, dimensions, float2(0.0f, -offset.y)));
    float w = RGBToLuminance(Sample(center, dimensions, float2(-offset.x, 0.0f)));
    
    float highest = max(max(max(max(n, e), s), w), m);
    float lowest = min(min(min(min(n, e), s), w), m);
    float contrast = highest - lowest;
    
    g_Texture[DTid.xy].xyzw = m;
}