struct Transformation {
    float4x4 worldViewProjMatrix;
};

ConstantBuffer<Transformation> transformation_ : register(b0);

struct VSInput {
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position.xyz, 1.0f), transformation_.worldViewProjMatrix);
    output.texcoord = input.texcoord;
    return output;
}