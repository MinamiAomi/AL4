#include "Pathtracing.h"

#define TRUE_UINT 1
#define FALSE_UINT 0

#define INVALID_COLOR float32_t3(-1.0f, -1.0f, -1.0f)

//#define USE_NORMAL_MAPS

//////////////////////////////////////////////////
// R + M * G + I HitGroupShaderRecordIndex      
// R = RayContributionToHitGroupIndex
// M = MultiplierForGeometryContributionToHitGroupIndex
// G = GeometryContributionToHitGroupIndex
// I = InstanceContributionToHitGroupIndex
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// 0 + 1 * G + I PrimaryHitGroup
#define RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 0
#define MULTIPLIER_FOR_GEOMETRY_CONTRIBUTION_TO_HIT_GROUP_INDEX 1
#define MISS_SHADER_INDEX 0
//////////////////////////////////////////////////


#define RAY_ATTRIBUTE (1 << 0)

struct Scene {
    float32_t4x4 viewProjectionInverseMatrix;
    float32_t3 cameraPosition;
};

struct Payload {
    float32_t3 color;
    uint32_t recursiveCount;
};

struct Attributes {
    float32_t2 barycentrics;
};

ConstantBuffer<Scene> g_Scene : register(b0);

RaytracingAccelerationStructure g_TLAS : register(t0);
RWTexture2D<float32_t4> g_Color : register(u0);
SamplerState g_PointSampler : register(s0);
SamplerState g_LinearSampler : register(s1);

Texture2D<float32_t4> g_BindlessTextures[] : register(t0, space1);