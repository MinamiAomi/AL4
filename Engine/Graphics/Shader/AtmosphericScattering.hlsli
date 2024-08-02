#define M_MAX 1e9
static const float32_t M_PI = 3.14159265359f;
static const float32_t M_4PI = 4.0f * M_PI;

static const float32_t Kr = 0.0025f;
static const float32_t Km = 0.0010f;
static const float32_t Exposure = 0.05f;

static const float32_t ESun = 1300.0f;
static const float32_t3 SunPosition = float32_t3(0.0f, 1.0f, 0.0f);

static const float32_t3 InvWaveLength = 1.0f / pow(float32_t3(0.680f, 0.550f, 0.440f), 4.0f);

static const float32_t EarthRadius = 6360.0f;
static const float32_t AtmosphereHeight = 60.0f;
static const float32_t AtmosphereRadius = EarthRadius + AtmosphereHeight;

static const float32_t Scale = 1.0f / AtmosphereHeight;
static const float32_t KrESun = Kr * ESun;
static const float32_t KmESun = Km * ESun;
static const float32_t Kr4PI = Kr * 4.0f * M_PI;
static const float32_t Km4PI = Km * 4.0f * M_PI;

static const float32_t ScaleDepth = 0.25f;
static const float32_t ScaleOverScaleDepth = Scale / ScaleDepth;
static const float32_t G = -0.999f;

float32_t3 IntersectionSphere(float32_t3 rayOrigin, float32_t3 rayDirection, float32_t sphereRadius) {
    float32_t a = dot(rayDirection, rayDirection);
    float32_t b = 2.0f * dot(rayOrigin, rayDirection);
    float32_t c = dot(rayOrigin, rayOrigin) - sphereRadius * sphereRadius;
    float32_t d = b * b - 4.0f * a * c;

    if (d < 0.0001f) { return float32_t3(0.0f, 0.0f, 0.0f); }
    return (rayOrigin + rayDirection * (0.5f * (-b + sqrt(d)) / a));
}

float32_t IntegralApproximation(float32_t cosine) {
    float32_t x = 1.0f - cosine;
    return ScaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
}

float32_t3 AtmosphericScattering(float32_t3 rayOrigin, float32_t3 rayDirection) {
    float32_t3 skyPosition = IntersectionSphere(rayOrigin, rayDirection, AtmosphereRadius);
    float32_t skyDistance = length(skyPosition - rayOrigin);

    float32_t height = length(rayOrigin) + 0.0001f;
    float32_t depth = exp(ScaleOverScaleDepth * (EarthRadius - length(rayOrigin)));
    float32_t angle = dot(rayOrigin, rayOrigin) / height;
    float32_t offset = depth * IntegralApproximation(angle);

    const int32_t numSamples = 5;
    float32_t sampleStep = skyDistance / (float32_t)numSamples;
    float32_t scaledSampleStep = sampleStep * Scale;
    float32_t3 samplePoint = rayOrigin + rayDirection * 0.5f;

    float32_t3 frontColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t i = 0; i < numSamples; ++i) {
        height = length(samplePoint) + 0.0001f;
        depth = exp(ScaleOverScaleDepth * (EarthRadius - height));
        float32_t sunAngle = dot(SunPosition, samplePoint) / height;
        float32_t rayAngle = dot(rayDirection, samplePoint) / height;
        float32_t scatter = (offset + depth * (IntegralApproximation(sunAngle) - IntegralApproximation(rayAngle)));
        float32_t3 attenuate = exp(-scatter * (InvWaveLength * Kr4PI + Km4PI));
        frontColor += attenuate * (depth * scaledSampleStep);
        samplePoint += sampleStep * rayDirection;
    }

    float32_t3 primaryColor = frontColor * (InvWaveLength * KrESun);
    float32_t3 secondaryColor = frontColor * KmESun;

    float32_t cosine = dot(SunPosition, normalize(rayOrigin - skyPosition));
    float32_t rayleightPhase = 0.75f * (1.0f * cosine * cosine);

    const float32_t G2 = G * G;
    float32_t miePhase = 1.5f * ((1.0f - G2) / (2.0f + G2)) * (1.0f + cosine * cosine) / pow(abs(1.0f + G2 - 2.0f * G * cosine), 1.5f);

    float32_t3 rayleightColor = primaryColor * rayleightPhase;
    float32_t3 mieColor = secondaryColor * miePhase;
    float32_t3 color = 1.0f - exp(-Exposure * (rayleightColor + mieColor));
    return frontColor;
}