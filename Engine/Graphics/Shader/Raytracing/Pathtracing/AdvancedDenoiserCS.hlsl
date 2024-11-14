// A-Trous Filter

#define NUM_THREADS 32

Texture2D<float32_t4> g_IntermediateBuffer : register(t0);
RWTexture2D<float32_t4> g_AccumulationBuffer : register(u0);
RWTexture2D<float32_t4> g_DenoisedBuffer : register(u0);
SamplerState g_Sampler : register(s0);

float32_t ComputeWeight(in int32_t2 offset) {
    const float32_t kernels[] = { 1.0f / 16.0f, 1.0f / 4.0f, 3.0f / 8.0f, 1.0f / 4.0f, 1.0f / 16.0f };
    return kernels[offset.x] * kernels[offset.y];
}

// 5 * 5 Filter
float32_t3 ATrousFilter(in float32_t2 centerUV, in float32_t2 pixelSize) {
    float32_t3 color = float32_t3(0.0f, 0.0f, 0.0f);
    float32_t weight = 1.0f;
    for (int32_t y = 0; y < 5; ++y) {
        for (int32_t x = 0; x < 5; ++x) {
            int32_t2 offset = int32_t2(x - 2, y - 2);
            float32_t2 sampleUV = centerUV + offset * pixelSize;
            float32_t w = ComputeWeight(offset);
            color += g_IntermediateBuffer.SampleLevel(g_Sampler, sampleUV, 0).rgb * w;
            weight += w;
        }
    }
    return color / weight;
}

[numthreads(NUM_THREADS, NUM_THREADS, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID, uint32_t3 GTid : SV_GroupThreadID) {

    int32_t2 targetPixel = (int32_t2)DTid.xy;
    float32_t2 textureSize;
    g_IntermediateBuffer.GetDimensions(textureSize.x, textureSize.y);

    // 範囲外の場合何もしない
    if (targetPixel.x >= textureSize.x || targetPixel.y >= textureSize.y) { return; }
    
    float32_t2 centerUV = (float32_t2)targetPixel / textureSize;
    float32_t2 pixelSize = 1.0f / textureSize;
    float32_t4 color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
    // フィルター
    color.rgb = ATrousFilter(centerUV, pixelSize);

    g_DenoisedBuffer[targetPixel] = color; 
}