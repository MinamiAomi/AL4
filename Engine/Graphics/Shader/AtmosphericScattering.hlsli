static const float32_t M_PI = 3.14159265359f;
static const float32_t Kr = 0.0025f;
static const float32_t Km = 0.0010f;
static const float32_t ESun = 1300.0f;

static const float32_t3 lightPos = float32_t3(0.0f, 1.0f, 0.0f);
static const float32_t3 invWaveLength = float32_t3(4.67696746925f, 10.9282152858f, 26.6802131001f);
static const float32_t innerRadius = 1025.0f;
static const float32_t innerRadius2 = innerRadius * innerRadius;
static const float32_t outerRadius = 1000.0f;
static const float32_t outerRadius2 = outerRadius * outerRadius;
static const float32_t scale = 1.0f / (outerRadius - innerRadius);
static const float32_t KrESun = Kr * ESun;
static const float32_t KmESun = Km * ESun;
static const float32_t Kr4PI = Kr * 4.0f * M_PI;
static const float32_t Km4PI = Km * 4.0f * M_PI;
static const float32_t scaleDepth = 0.25f;
static const float32_t scaleOverScaleDepth = scale / scaleDepth;
static const float32_t g = -0.999f;
static const float32_t exposure = 0.05f;

float32_t3 IntersectionPos(float32_t3 rayPos, float32_t3 rayDir) {
    float32_t A = dot(rayDir, rayDir);
    float32_t B = 2.0f * dot(rayPos, rayDir);
    float32_t C = dot(rayPos, rayPos) - outerRadius * outerRadius;

    float32_t D = B * B - 4.0f * A * C;
    if (D < 0.000001f) {
        return float32_t3(0.0f, 0.0f, 0.0f);
    }
    return (rayPos + rayDir * (0.5f * (-B + sqrt(B * B - 4.0f * A * C)) / A));
}

float32_t IntegralApproximation(float32_t cosine) {
    float32_t x = 1.0f - cosine;
    return scaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
}

float32_t3 AtmosphericScattering(float32_t3 rayPos, float32_t3 rayDir) {
    float32_t3 skyPos = IntersectionPos(rayPos, rayDir);

    float32_t cameraHeight = length(rayPos);
    
    float32_t far = length(skyPos - rayPos);
    float32_t height = length(rayPos);
    float32_t depth = exp(scaleOverScaleDepth * (innerRadius - cameraHeight));
    float32_t startAngle = dot(rayDir, rayPos) / height;
    float32_t startOffset = depth * IntegralApproximation(startAngle);

    const int32_t numSamples = 5;
    float32_t sampleLength = far / (float32_t)numSamples;
    float32_t scaledLength = sampleLength * scale;
    float32_t3 sampleRay = rayDir * sampleLength;
    float32_t3 samplePoint = rayPos + sampleRay * 0.5f;
    
    
    float32_t3 frontColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t i = 0; i < numSamples; ++i) {
        height = length(samplePoint);
        depth = exp(scaleOverScaleDepth * (innerRadius - height));
        float32_t lightAngle = dot(lightPos, samplePoint) / height;
        float32_t cameraAngle = dot(rayDir, samplePoint) / height;
        float32_t scatter = (startOffset + depth * (IntegralApproximation(lightAngle) - IntegralApproximation(cameraAngle)));
        float32_t3 attenuate = exp(-scatter * (invWaveLength * Kr4PI + Km4PI));
        frontColor += attenuate * (depth * scaledLength);
        samplePoint += sampleRay;
    }

    float32_t3 primaryColor = frontColor * (invWaveLength * KrESun);
    float32_t3 secondaryColor = frontColor * KmESun;
    float32_t3 direction = rayPos - skyPos;

    float32_t cosine = dot(lightPos, direction) / length(direction);
    float32_t rayPhase = 0.75f * (1.0f + cosine * cosine);

    float32_t g2 = g * g;
    float32_t miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + cosine * cosine) / pow(abs(1.0f + g2 - 2.0f * g * cosine), 1.5f);

    float32_t3 rayColor = primaryColor * rayPhase;
    float32_t3 mieColor = secondaryColor * miePhase;
    return saturate(1.0f - exp(-exposure * (rayColor + mieColor)));
}
