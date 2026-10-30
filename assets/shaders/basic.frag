#version 330 core
out vec4 FragColor;
in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec2 vUV;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float linear;
    float quadratic;
};
uniform PointLight uLights[4];
uniform vec3 uAmbientColor;
uniform vec3 uBaseColor;
uniform vec3 uEmission;
uniform float uShininess;
uniform vec2 uUvScale;
uniform bool uUseTexture;
uniform sampler2D uTexture;
uniform vec3 uViewPosition;

void main() {
    vec3 albedo = uBaseColor;
    if (uUseTexture) albedo *= texture(uTexture, vUV * uUvScale).rgb;

    // Based on LearnOpenGL Basic Lighting / Multiple Lights; adapted for
    // our own Blinn-Phong point-light structure, color and attenuation.
    vec3 N = normalize(vWorldNormal);
    vec3 V = normalize(uViewPosition - vWorldPosition);
    vec3 result = uAmbientColor * albedo + uEmission;

    for (int i = 0; i < 4; ++i) {
        if (uLights[i].intensity <= 0.0) continue;
        vec3 direction = uLights[i].position - vWorldPosition;
        float d = length(direction);
        vec3 L = direction / max(d, 0.0001);
        float lambert = max(dot(N, L), 0.0);
        vec3 H = normalize(L + V);
        float specular = lambert > 0.0
            ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;
        float attenuation = 1.0 / (1.0 + uLights[i].linear * d +
                                   uLights[i].quadratic * d * d);
        vec3 radiance = uLights[i].color * uLights[i].intensity * attenuation;
        result += radiance * (lambert * albedo + 0.16 * specular);
    }
    // Simple display-friendly mapping avoids white clipping from strong lights.
    result = result / (result + vec3(1.0));
    FragColor = vec4(pow(result, vec3(1.0 / 2.2)), 1.0);
}
