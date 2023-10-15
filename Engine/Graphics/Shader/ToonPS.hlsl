struct Scene
{
    float4x4 viewProjMatrix;
    float3 cameraPosition;
};
ConstantBuffer<Scene> scene_ : register(b0);

struct Material
{
    float3 diffuse;
    float3 specular;
};
ConstantBuffer<Material> material_ : register(b2);

// 平行光源
struct DirectionalLight
{
    float3 direction; // 方向
    float3 color; // 色
    float intensity; // 強さ
};

//ConstantBuffer<DirectionalLight> directionalLight_ : register(b3);

Texture2D<float4> texture_ : register(t0);
SamplerState sampler_ : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

float ToonDiffuse(float3 normal, float3 lightDirection)
{
    float t = -dot(normal, lightDirection);
    t = t * 0.5f + 0.5f;
    return lerp(1.0f, 0.2f, step(t, 0.4f));
    //float2 toonShadeUV = float2(t * 0.5f + 0.5f, 0.0f);
    //return toonShadeTexture_.Sample(toonShadeSampler_, toonShadeUV).r;
    //return t;
}

float ToonSpecular(float3 normal, float3 pixelToCamera, float3 lightDirection)
{
    const float shininess = 10.0f;
    const float threshold = 0.5f;
    float3 reflectVec = reflect(lightDirection, normal);
    float t = pow(saturate(dot(reflectVec, pixelToCamera)), shininess);
    return step(threshold, t);
    //return t;
}

PSOutput main(PSInput input)
{
    // 位置
    float3 position = input.worldPosition;
    // 法線
    float3 normal = normalize(input.normal);
    // ピクセルからカメラへのベクトル 
    float3 pixelToCamera = normalize(scene_.cameraPosition - position);
    
    DirectionalLight directionalLight_;
    directionalLight_.direction = normalize(float3(1.0f, -1.0f, 0.0f));
    directionalLight_.intensity = 1.0f;
    directionalLight_.color = float3(1.0f, 1.0f, 1.0f);
    
    // テクスチャの色
    float3 textureColor = texture_.Sample(sampler_, input.texcoord).rgb;
    //float3 textureColor = float3(0.6f, 0.6f, 0.6f);
    // 拡散反射
    float3 diffuse = material_.diffuse * ToonDiffuse(normal, directionalLight_.direction);
    // 鏡面反射
    float3 specular = material_.specular * ToonSpecular(normal, pixelToCamera, directionalLight_.direction);
    // シェーディングによる色
    float3 shadeColor = (diffuse + specular) * directionalLight_.color * directionalLight_.intensity;
    
    
    PSOutput output;
    output.color.rgb = textureColor * shadeColor;
    output.color.a = 1.0f;
    
    //output.color.rgb = specular;
    
    return output;
}