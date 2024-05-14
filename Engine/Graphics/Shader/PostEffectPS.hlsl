struct Constant {
    float32_t3 grayscaleColor;
    uint32_t useGrayscale;
    float32_t vignetteIntensity;
    float32_t vignettePower;
    uint32_t useVignette;
};
ConstantBuffer<Constant> g_Constant : register(b0);
Texture2D<float32_t4> g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PSInput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float32_t4 color : SV_TARGET0;
};

float3 LinearToSRGB(float3 color) {
    float3 sqrt1 = sqrt(color);
    float3 sqrt2 = sqrt(sqrt1);
    float3 sqrt3 = sqrt(sqrt2);
    float3 srgb = 0.662002687 * sqrt1 + 0.684122060 * sqrt2 - 0.323583601 * sqrt3 - 0.0225411470 * color;
    return srgb;
}
    
float32_t3 Grayscale(float32_t3 color) {
    float32_t3 k = float32_t3(0.2125f, 0.7154f, 0.0721f);
    return dot(color, k) * g_Constant.grayscaleColor;
}

float32_t Vignette(float32_t2 texcoord) {
    float32_t2 correct = texcoord * (1.0f - texcoord.yx);
    float32_t vignette = correct.x * correct.y * g_Constant.vignetteIntensity;
    return saturate(pow(vignette, g_Constant.vignettePower));
}

PSOutput main(PSInput input) {
    PSOutput output;

    float32_t4 outputColor = g_Texture.Sample(g_Sampler, input.texcoord);
    float32_t3 grayscale = Grayscale(outputColor.rgb);
    outputColor.rgb = lerp(outputColor.rgb, grayscale, g_Constant.useGrayscale);
    
    float32_t vignette = Vignette(input.texcoord);
    outputColor.rgb *= lerp(1.0f, vignette, g_Constant.useVignette);
    
    output.color = outputColor;

    return output;
}