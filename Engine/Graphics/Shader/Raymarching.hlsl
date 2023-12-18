
#define MAX_STEPS 16
#define MAX_DISTANCE 100
#define EPSILON 0.001f

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

float3 Trans(float3 p) {
    return fmod(p, 4.0f) - 2.0f;
}

float GetDistance(in float3 position) {
    float3 p = float3(0.0f, 1.0f, 0.0f);
    float r = 1.0f;
    
    float distance = length(Trans(position - p)) - r;
    
    return distance;
}

float Raymarch(in float3 rayOrigin, in float3 rayDirection) {
    
    float distance = 0.0f;
    
    for (uint i = 0; i < MAX_STEPS; ++i) {
        for (uint i = 0; i < MAX_STEPS; ++i) {
            float3 position = rayOrigin + rayDirection * distance;
            float d = GetDistance(position);
            distance += d;
        
            if (distance > MAX_DISTANCE || d < EPSILON) {
                break;
            }
        }
    }

    return distance;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

    float2 dimensions;
    g_Output.GetDimensions(dimensions.x, dimensions.y);
    
    float2 texcoord = ((float2) DTid.xy + 0.5f) / dimensions;
    float3 nearPosition = GetWorldPosition(texcoord, 0.0f, g_Scene.viewProjectionInverse);
    float3 farPosition = GetWorldPosition(texcoord, 1.0f, g_Scene.viewProjectionInverse);
    
    float3 rayOrigin = nearPosition;
    float3 rayDirection = normalize(farPosition - nearPosition);
    float distance = Raymarch(rayOrigin, rayDirection);
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (distance < MAX_DISTANCE) {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }  

    g_Output[DTid.xy] = color;
}