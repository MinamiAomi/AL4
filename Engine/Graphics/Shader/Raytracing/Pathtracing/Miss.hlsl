#include "Pathtracing.hlsli"
#include "../../AtmosphericScattering.hlsli"

TextureCube<float32_t4> l_Skybox : register(t0, space3);

[shader("miss")]
void RecursiveMiss(inout Payload payload) {
    float32_t3 rayDirection = WorldRayDirection();
    payload.color = clamp(l_Skybox.SampleLevel(g_LinearSampler, rayDirection, 0).rgb, 0.0f, 10.0f);
    //payload.color = AtmosphericScattering(WorldRayOrigin(), rayDirection);
}