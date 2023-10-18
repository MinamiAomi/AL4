struct Surface {
    float3 position;
    float3 normal;
    float3 directionToCamera;
    float3 distanceToCamera;
};

struct DirectionalLight {
    float3 direction;
    float3 color;
    float intensity;
};

struct PointLight {
    float3 position;
    float3 attenuation;
    float3 color;
    float intensity;
};

struct SpotLight {
    float3 position;
    float3 direction;
    float3 attenuation;
    float attenuationStartAngleCos;
    float attenuationEndAngleCos;
    float3 color;
    float3 intensity;
};

float DistanceAttenuation(in float distance, in float3 attenuation) {
    float3 factor = float3(1.0f, distance, distance * distance);
    return 1.0f / dot(attenuation, factor);
}

float AngleAttenuation(in float3 directionToLight, in float3 lightDirection, in float startAngleCos, in float endAngleCos) {
    float cos = dot(directionToLight, lightDirection);
    return smoothstep(endAngleCos, startAngleCos, cos);
}

float LambertReflection(in float3 normal, in float3 lightDirection) {
    return saturate(dot(normal, -lightDirection));
}

float HalfLambertReflection(in float3 normal, in float3 lightDirection) {
    return pow(dot(normal, -lightDirection) * 0.5f + 0.5f, 2.0f);
}

float PhongReflection(in float3 normal, in float3 lightDirection, in float3 toCamera, in float shininess) {
    float3 reflectVec = reflect(-lightDirection, normal);
    return pow(saturate(dot(reflectVec, toCamera)), shininess);
}

float BlinnPhongReflection(in float3 normal, in float3 lightDirection, in float3 toCamera, in float shininess) {
    float3 halfVec = normalize(-lightDirection + toCamera);
    return pow(saturate(dot(normal, halfVec)), shininess);
}