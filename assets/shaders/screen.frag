#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uCameraFeed;
void main() {
    // Color texture was rendered by a second camera this frame.
    // Render it unlit so the screen remains bright in night mode.
    vec3 imageColor = texture(uCameraFeed, vUV).rgb;
    FragColor = vec4(imageColor, 1.0);
}
