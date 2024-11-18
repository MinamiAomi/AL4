#define NUM_THREADS 32

Texture2D<float32_t4> g_IntermadiateBuffer : register(t0);
RWTexture2D<float32_t4> g_AccumulationBuffer : register(u0);
RWTexture2D<float32_t4> g_DenoisedBuffer : register(u1);

struct Common {
    uint32_t sampleCount;
};
ConstantBuffer<Common> g_Common : register(b0);

[numthreads(NUM_THREADS, NUM_THREADS, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID, uint32_t3 GTid : SV_GroupThreadID) {

    uint32_t2 targetPixel = DTid.xy;

    uint32_t2 textureSize;
    g_IntermadiateBuffer.GetDimensions(textureSize.x, textureSize.y);

     // テクスチャ外の場合何もしない
    if (targetPixel.x >= textureSize.x || targetPixel.y >= textureSize.y) { return; }

    float32_t3 color = g_IntermadiateBuffer[targetPixel].rgb; 
    float32_t3 accumulationColor = g_AccumulationBuffer[targetPixel].rgb;

    accumulationColor += color;
    
    g_AccumulationBuffer[targetPixel].rgb = accumulationColor;
    g_AccumulationBuffer[targetPixel].a = 1.0f;

    accumulationColor /= g_Common.sampleCount;

    g_DenoisedBuffer[targetPixel].rgb = accumulationColor;
    g_DenoisedBuffer[targetPixel].a = 1.0f;
}