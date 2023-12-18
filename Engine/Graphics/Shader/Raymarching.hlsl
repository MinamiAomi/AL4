
struct Scene {
    float4x4 viewProjectionInverse;
};
ConstantBuffer<Scene> g_Scene : register(b0);
RWTexture2D<float4> g_Output : register(u0);

// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    position.xyz /= position.w;
    return position.xyz;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

    
    
    float4 color;
    

    g_Output[DTid.xy] = color;
}