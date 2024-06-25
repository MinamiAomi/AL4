static const float32_t PI = 3.14159265359f;
static const float32_t INV_PI = 0.31830988618f;
static const float32_t EPSILON = 0.00001f;

namespace PBR {

    // 入射光
    struct IncidentLight {
        float32_t3 direction;
        float32_t3 color;
    };
    // 反射光
    struct ReflectedLight {
        float32_t3 directDiffuse;
        float32_t3 directSpecular;
    };
    
    struct Geometry {
        float32_t3 position;
        float32_t3 normal;
        float32_t3 viewDirection;
    };

    struct Material {
        float32_t3 diffuseReflectance;
        float32_t3 specularReflectance;
        float32_t3 specularRoughness;
    };


    float32_t Pow5(in float32_t n) {
        float32_t n2 = n * n;
        return n2 * n2 * n;
    }

    float32_t3 SchlickFresnel(in float32_t3 f0, in float32_t3 f90, in float32_t cosine) {
        return lerp(f0, f90, Pow5(1.0f - cosine));
    }

    // GGX法線分布関数
    float32_t D_GGX(in float32_t NdotH, in float32_t alpha) {
        float32_t a2 = alpha * alpha;
        float32_t t = (NdotH * NdotH * (a2 - 1.0f)) + 1.0f;
        return a2 / (t * t * PI);
    }

    // GGXシャドウマスキング関数
    float32_t G_Smith_Schlick_GGX(in float32_t NdotV, in float32_t NdotL, in float32_t alpha) {
        float32_t k = alpha * 0.5f + EPSILON;
        float32_t GV = NdotV / (NdotV * (1.0f - k) + k);
        float32_t GL = NdotL / (NdotL * (1.0f - k) + k);
        return GV * GL;
    }

    float32_t3 F_Schlick(in float32_t3 specularReflectance, in float32_t LdotH) {
        return (specularReflectance + (1.0f - specularReflectance) * Pow5(1.0f - LdotH));
    }

    float32_t3 DiffuseBRDF(in float32_t3 diffuseReflectance) {
        return diffuseReflectance * INV_PI;
    }

    float32_t3 SpecularBRDF(in float32_t3 incidentDirection, in float32_t3 normal, in float32_t3 viewDirection, in float32_t3 specularReflectance, in float32_t specularRoughness) {
        float32_t3 N = geometry.normal;
        float32_t3 V = geometry.viewDirection;
        float32_t3 L = incidentDirection;
        float32_t3 H = normalize(L + V);
        

        float32_t NdotL = saturate(dot(N, V));
        float32_t NdotL = saturate(dot(N, L));
        float32_t LdotH = saturate(dot(L, H));
        float32_t NdotH = saturate(dot(N, H));

        float32_t alpha = specularRoughness * specularRoughness;
        float32_t3 F = F_Schlick(specularReflectance, LdotH);
        float32_t D = D_GGX(NdotH, alpha);
        float32_t G = G_Smith_Schlick_GGX(NdotV, NdotL, alpha);
        return (F * (D * G)) / (4.0f * NdotV * NdotL + EPSILON);
    }

    void DirectRenderingEquations(in IncidentLight directLight, in Geometry geometry, in Material material, inout ReflectedLight reflectedLight) {
        float32_t NdotL = saturate(dot(geometry.normal, directLight.direction));
        float32_t3 irradiance = directLight.color * NdotL;
        irradiance *= PI;
        
        reflectedLight.directDiffuse += irradiance * DiffuseBRDF(material.diffuseReflectance);
        reflectedLight.directDiffuse += irradiance * SpecularBRDF(directLight.direction, geometry.nromal, gometry.viewDirection, material.specularReflectance, material.specularRoughness);
    }

}
