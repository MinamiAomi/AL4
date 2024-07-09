
float32_t Random3dTo1d(float32_t3 value, float32_t3 mutator = float32_t3(12.9898f, 78.233f, 37.719f)) {
    return frac(sin(dot(sin(value), mutator)) * 143758.5453f);
}

float32_t Random2dTo1d(float32_t2 value, float32_t2 mutator = float32_t2(12.9898f, 78.233f)) {
    return frac(sin(dot(sin(value), mutator)) * 143758.5453f);
}


float32_t Random1dTo1d(float32_t value, float32_t mutator = 0.546f) {
    return frac(sin(value + mutator) * 143758.5453f);
}
