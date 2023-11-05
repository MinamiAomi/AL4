

float3 HomogeneousCoordinateTransform(in float3 v, in float4x4 m) {
    float4 tmp = mul(float4(v, 1.0f), m);
    tmp.xyz *= 1.0f / tmp.w;
    return tmp.xyz;
}

float2 LaunchIndexToNDC(float2 launchIndex, float2 dimensions) {
    float2 ndc = float2(launchIndex) + 0.5f;
    ndc *= float2(2.0f, -2.0f) / dimensions;
    ndc += float2(-1.0f, 1.0f);
    return ndc;
}