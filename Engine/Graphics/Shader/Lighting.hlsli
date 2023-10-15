// 面
struct Surface {
    float3 position;            // 座標
    float3 normal;              // 法線
    float3 directionToCamera;   // カメラへの方向
    float3 distanceToCamera;    // カメラとの距離
};

// 平行光源
struct DirectionalLight {
    float3 direction;   // 方向
    float3 color;       // 色
    float  intensity;   // 強さ
};

// 点光源
struct PointLight {
    float3 position;    // 座標
    float3 attenuation; // 減衰
    float3 color;       // 色
    float  intensity;   // 強さ
};

// スポットライト
struct SpotLight {
    float3 position;                    // 座標
    float3 direction;                   // 方向
    float3 attenuation;                 // 減衰
    float  attenuationStartAngleCos;    // 減衰開始角度
    float  attenuationEndAngleCos;      // 減衰終了角度
    float3 color;                       // 色
    float3 intensity;                   // 強さ
};

// 距離減衰
float DistanceAttenuation(in float distance, in float3 attenuation) {
    float3 factor = float3(1.0f, distance, distance * distance);
    return 1.0f / dot(attenuation, factor);
}

// 角度減衰
float AngleAttenuation(in float3 directionToLight, in float3 lightDirection, in float startAngleCos, in float endAngleCos) {
    float cos = dot(directionToLight, lightDirection);
    return smoothstep(endAngleCos, startAngleCos, cos);
}