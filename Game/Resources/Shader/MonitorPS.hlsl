Texture2D<float4> texture_ : register(t0);
SamplerState sampler_ : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

float3 LinearToSRGB(float3 color) {
    float3 sqrt1 = sqrt(color);
    float3 sqrt2 = sqrt(sqrt1);
    float3 sqrt3 = sqrt(sqrt2);
    float3 srgb = 0.662002687 * sqrt1 + 0.684122060 * sqrt2 - 0.323583601 * sqrt3 - 0.0225411470 * color;
    return srgb;
}
    

PSOutput main(PSInput input) {
    PSOutput output;

    float2 texcoord = input.texcoord;
    texcoord -= 0.5f;
    
    float distort = 0.1f;
    
    texcoord /= 1.0f - length(texcoord) * distort;
    
    //float power = pow(length(texcoord), distort);
    //texcoord *= power;
    //texcoord *= 2.25f;
    texcoord *= 1.25f;
    //texcoord.y *= 0.97f;
    texcoord += 0.5f;

    output.color = texture_.Sample(sampler_, texcoord);
    
    //output.color *= 1.5f;

    return output;
}
