RWTexture2D<float4> g_Texture : register(u0);

float GetRandom(float2 uv, float seed) {
    return frac(sin(dot(uv, float2(12.9898f, 78.233f)) + seed) * 43758.5453f);
}

float GetLerpRandom(float2 uv, float density, float seed) {
    float invDensity = 1.0f / density;
    float2 block = floor(uv * density);
    float2 pixel = frac(uv);
    
    float v00 = GetRandom(uv + float2(0.0f, 0.0f), 0.0f);
    float v01 = GetRandom(uv + float2(0.0f, 1.0f), 0.0f);
    float v10 = GetRandom(uv + float2(1.0f, 0.0f), 0.0f);
    float v11 = GetRandom(uv + float2(1.0f, 1.0f), 0.0f);
    
    float v0010 = lerp(v00, v10, pixel.x);
    float v0111 = lerp(v01, v11, pixel.x);
    
    float random = lerp(v0010, v0111, pixel.y);
    
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    float2 textureSize;
    g_Texture.GetDimensions(textureSize.x, textureSize.y);
    float2 uv = DTid.xy / textureSize;
    // 密度
    float density = 10.0f;
    uv = uv * density;
    float2 pixel = frac(uv);
    float2 block = floor(uv);
    
    float v00 = GetRandom(uv + float2(0.0f, 0.0f), 0.0f);
    float v01 = GetRandom(uv + float2(0.0f, 1.0f), 0.0f);
    float v10 = GetRandom(uv + float2(1.0f, 0.0f), 0.0f);
    float v11 = GetRandom(uv + float2(1.0f, 1.0f), 0.0f);
    
    float v0010 = lerp(v00, v10, pixel.x);
    float v0111 = lerp(v01, v11, pixel.x);
    
    float random = lerp(v0010, v0111, pixel.y);
    
    g_Texture[DTid.xy] = float4(random, random, random, 1);
}