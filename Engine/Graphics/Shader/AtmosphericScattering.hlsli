#define M_MAX 1e9
static const float32_t M_PI = 3.14159265359f;
static const float32_t M_4PI = 4.0f * M_PI;

static const float32_t earthRadius = 6360.0f;
static const float32_t atmosphereHeight = 60.0f;
static const float32_t atmosphereRadius = earthRadius + atmosphereHeight;
static const float32_t3 earthCenter = float32_t3(0.0f, 0.0f, 0.0f);

static const float32_t distanceToSun = 1.496e8f;
static const float32_t sunRadius = 2.0f * 109.0f * earthRadius;
static const float32_t sunIntensity = 10.0f;

static const float32_t3 betaRayleight = float32_t3(5.8e-4f, 1.35e-3f, 3.31e-3f);
static const float32_t3 betaMie = float32_t3(4.0e-3f, 4.0e-3f, 4.0e-3f);

static const float32_t3 M_4PIBetaRayleight = M_4PI * betaRayleight;
static const float32_t3 M_4PIBetaMie = M_4PI * betaMie;
static const float32_t heightScaleRayleight = 6.0f;
static const float32_t heightScaleMie = 1.2f;
static const float32_t g = -0.76f;

static const float32_t numDensitySamples = 8.0f;
static const float32_t numViewSamples = 8.0f;
static const int32_t intNumDensitySamples = int32_t(numDensitySamples);
static const int32_t intNumViewSamples = int32_t(numViewSamples);

float32_t2 IntersectionSphere(float32_t3 rayOrigin, float32_t3 rayDirection, float32_t3 sphereCenter, float32_t sphereRadius) {
    float32_t3 tmp = rayOrigin - sphereCenter;
    float32_t b = dot(rayDirection, tmp);
    float32_t c = dot(tmp, tmp) - sphereRadius * sphereRadius;
    float32_t d = b * b - c;

    if (d < 0.0f) { return float32_t2(-M_MAX, -M_MAX); }

    float32_t dSq = sqrt(d);
    return float32_t2(-b - dSq, -d + dSq);
}

float32_t Phase(float32_t nu, float32_t g) {
    return (3.0f * (1.0f - g * g) * (1.0f + nu * nu)) / (2.0f * (2.0f + g * g) * pow(1.0f + g * g - 2.0f * g * nu, 1.5f));
}

float32_t2 DensityOverPath(float32_t3 p0, float32_t3 p1, float32_t2 prescaler) {
    float32_t l = length(p1 - p0);
    float32_t3 v = (p1 - p0) / l;

    l /= numDensitySamples;

    float32_t2 density = float32_t2(0.0f, 0.0f);
    float32_t t = 0.0f;
    for (int32_t i = 0; i < intNumDensitySamples; ++i) {
        float32_t3 sp = p0 + v * (t + 0.5f * l);
        float32_t2 h = length(sp) - earthRadius;
        density += exp(-h / prescaler);
        t += l;
    }
    return l * density;
}

float32_t4 Inscatter(float32_t3 rayOrigin, float32_t3 rayDirection, float32_t3 sunDirection) {
    float32_t2 t0 = IntersectionSphere(rayOrigin, rayDirection, earthCenter, atmosphereRadius);
    float32_t2 t1 = IntersectionSphere(rayOrigin, rayDirection, earthCenter, earthRadius);

    uint32_t noPlanetIntersection = (uint32_t)(t1.x < 0.0f && t1.y < 0.0f);
    
    float32_t farPoint = noPlanetIntersection == 1 ? t0.y : t1.x;
    float32_t nearPoint = t0.x > 0.0f ? t0.x : 0.0f;

    float32_t l = (farPoint - nearPoint) / numViewSamples;
    rayOrigin += nearPoint * rayDirection;

    float32_t t = 0.0f;
    float32_t3 rayleight = float32_t3(0.0f, 0.0f, 0.0f);
    float32_t3 mie = float32_t3(0.0f, 0.0f, 0.0f);

    float32_t2 prescalers = float32_t2(heightScaleRayleight, heightScaleMie);

    float32_t2 densityPointToRayOrigin = float32_t2(0.0f, 0.0f);
    for (int32_t i = 0; i < intNumViewSamples; ++i) {
        float32_t3 sp = rayOrigin + rayDirection * (t + 0.5f * l);
        float32_t tc = IntersectionSphere(sp, sunDirection, earthCenter, atmosphereRadius).y;
        float32_t3 pc = sp + tc * sunDirection;
        float32_t2 densitySPRayOrigin = DensityOverPath(sp, rayOrigin, prescalers);
        float32_t2 densities = DensityOverPath(sp, pc, prescalers);

        float32_t2 h = length(sp) - earthRadius;
        float32_t2 expRM = exp(-h / prescalers);

        rayleight += expRM.x * exp(-M_4PIBetaRayleight * densities.x);
        mie += expRM.y * exp(-M_4PIBetaMie * densities.y);

        densityPointToRayOrigin += densitySPRayOrigin;
        t += l;
    }

    rayleight *= l;
    mie *= l;

    float32_t3 extinction = exp(-(M_4PIBetaRayleight * densityPointToRayOrigin.x + M_4PIBetaMie * densityPointToRayOrigin.y));
    float32_t nu = dot(sunDirection, -rayDirection);
    float32_t3 inscatter = sunIntensity * (betaMie * mie * Phase(nu, g) + betaRayleight * Phase(nu, 0.0f) * rayleight);
    return float32_t4(inscatter, extinction.r * float32_t(noPlanetIntersection));
}

