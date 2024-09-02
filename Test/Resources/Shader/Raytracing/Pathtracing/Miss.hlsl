#include "Pathtracing.hlsli"
#include "../../AtmosphericScattering.hlsli"

ConstantBuffer<SkyParameter> l_SkyParameter : register(b0, space3);

TextureCube<float32_t4> l_Skybox : register(t0, space3);
TextureCube<float32_t4> l_SkyboxRadiance : register(t1, space3);

[shader("miss")]
void RecursiveMiss(inout Payload payload) {
    float32_t3 rayDirection = WorldRayDirection();
    float32_t3 rayOrigin = WorldRayOrigin();
    
    //if(payload.reflected == 1) {    
    //    payload.color = clamp(l_SkyboxRadiance.SampleLevel(g_LinearSampler, rayDirection, payload.skyboxLod).rgb, 0.0f, 10.0f);
    //} 
    //else {
    //    payload.color = clamp(l_Skybox.SampleLevel(g_LinearSampler, rayDirection, 0).rgb, 0.0f, 10.0f);
    //}
    payload.color = AtmosphericScattering(rayOrigin, normalize(rayDirection), l_SkyParameter).rgb;
}     