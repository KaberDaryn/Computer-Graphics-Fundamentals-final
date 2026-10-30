#version 330 core
out vec4 FragColor;
in vec3 vWorldPosition;
in vec3 vWorldNormal;

uniform float uTime;
uniform float uRgbSpeed;
uniform float uRgbPhase;
uniform int uRgbMode;  // 0 = rainbow, 1 = static, 2 = off
uniform vec3 uRgbStaticColor;
uniform float uRgbIntensity; // Stage 6 finale brightness from CPU state machine

void main() {
    // Original procedural RGB effect. The strip is a continuous wave along
    // world X, while each keyboard key adds its own constant phase offset.
    const float TAU = 6.28318530718;
    float phase = uTime * uRgbSpeed * 0.16
                + uRgbPhase + vWorldPosition.x * 0.36;
    vec3 rainbow = 0.5 + 0.5 * cos(TAU *
                   (vec3(phase) + vec3(0.0, 1.0 / 3.0, 2.0 / 3.0)));
    vec3 lightColor = uRgbMode == 0 ? rainbow : uRgbStaticColor;
    if (uRgbMode == 2) lightColor = vec3(0.006, 0.009, 0.013);

    // Gentle edge shading preserves each key's geometric shape even when lit.
    vec3 normal = normalize(vWorldNormal);
    float facing = 0.72 + 0.28 * max(dot(normal,
                       normalize(vec3(0.3, 0.8, 0.4))), 0.0);
    vec3 radiance = lightColor * 1.9 * facing * uRgbIntensity;
    radiance = radiance / (radiance + vec3(1.0));
    FragColor = vec4(pow(radiance, vec3(1.0 / 2.2)), 1.0);
}
