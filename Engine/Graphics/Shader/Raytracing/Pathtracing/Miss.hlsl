#include "Pathtracing.hlsli"

TextureCube<float32_t4> l_Skybox : register(t0, space3);

[shader("miss")]
void RecursiveMiss(inout Payload payload) {
    float32_t3 rayDirection = WorldRayDirection();
    payload.color = l_Skybox.SampleLevel(g_LinearSampler, rayDirection, 0).rgb;
    //payload.color = float32_t3(1.0f, 1.0f, 1.0f);
}