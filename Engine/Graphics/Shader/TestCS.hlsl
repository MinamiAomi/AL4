RWTexture2D<float4> g_Texture : register(u0);

float GetRandom(float2 uv, float seed) {
    return frac(sin(dot(uv, float2(12.9898f, 78.233f)) + seed) * 43758.5453f);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    float2 textureSize;
    g_Texture.GetDimensions(textureSize.x, textureSize.y);
    float2 uv = DTid.xy / textureSize;
    float random = GetRandom(uv, 0.0f);
    g_Texture[DTid.xy] = float4(random, random, random, 1);
}