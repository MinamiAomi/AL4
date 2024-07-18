// 四隅のピクセルを共有メモリに格納して
// フィルタに通します


#define NUM_THREADS 32

Texture2D<float32_t4> g_IntermadiateBuffer : register(t0);
RWTexture2D<float32_t4> g_AccumulationBuffer : register(u0);
RWTexture2D<float32_t4> g_DenoisedBuffer : register(u1);

struct Common {
    uint32_t sampleCount;
};
ConstantBuffer<Common> g_Common : register(b0);

static const float32_t kKernelFactors[3][3] = {
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
};
// 共有メモリ
// 3x3フィルタのため左右上下1マス多め
groupshared float32_t3 sharedMemory[NUM_THREADS + 2][NUM_THREADS + 2];

[numthreads(NUM_THREADS, NUM_THREADS, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID, uint32_t3 GTid : SV_GroupThreadID) {

    uint32_t2 targetPixel = DTid.xy;

    uint32_t2 textureSize;
    g_IntermadiateBuffer.GetDimensions(textureSize.x, textureSize.y);

    uint32_t2 sharedMemoryCenter = GTid.xy + 1;

    // テクスチャ外の場合何もしない
    if (targetPixel.x >= textureSize.x || targetPixel.y >= textureSize.y) { return; }

    const int32_t2 offset[4] = {
        int32_t2(-1, 1),
        int32_t2( 1, 1),
        int32_t2( 1,-1),
        int32_t2(-1,-1),
    };

    // 自分のピクセルの四隅をサンプリングする
    for (uint32_t i = 0; i < 4; ++i) {
        uint32_t2 samplePixel = (uint32_t2)clamp((int32_t2)targetPixel + offset[i], int32_t2(0, 0), int32_t2(textureSize - 1));
        uint32_t2 sharedMemoryPosition = sharedMemoryCenter + offset[i];
        float32_t3 sampleColor = g_IntermadiateBuffer[samplePixel].rgb;
        sharedMemory[sharedMemoryPosition.y][sharedMemoryPosition.x] = sampleColor;
    }

    // グループ内のすべてのスレッドが完了するのを待つ
    GroupMemoryBarrierWithGroupSync();

    // 計算する
    float32_t3 totalColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (uint32_t y = 0; y < 3; ++y) {
        for (uint32_t x = 0; x < 3; ++x) {
            uint32_t2 sharedMemoryPosition = sharedMemoryCenter + uint32_t2(x - 1, y - 1);
            totalColor += sharedMemory[sharedMemoryPosition.y][sharedMemoryPosition.x] * kKernelFactors[y][x];
        }
    }

    float32_t3 accumulationColor = g_AccumulationBuffer[targetPixel].rgb * (g_Common.sampleCount - 1);
    accumulationColor += totalColor;
    accumulationColor /= g_Common.sampleCount;

    g_AccumulationBuffer[targetPixel].rgb = accumulationColor;
    g_AccumulationBuffer[targetPixel].a = 1.0f;


    float32_t3 b = accumulationColor /*/ (1.0f + accumulationColor)*/;
   /* 
    if (b.r > 1.0f || b.g > 1.0f || b.b > 1.0f) {
        b = float32_t3(1.0f, 0.0f, 0.0f);
    }*/

    g_DenoisedBuffer[targetPixel].rgb = b;
    g_DenoisedBuffer[targetPixel].a = 1.0f;
}