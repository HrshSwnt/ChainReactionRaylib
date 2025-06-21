#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in float glowStrength;

uniform vec3 baseColor;

out vec4 finalColor;

void main() {
    // Use the interpolated glowStrength from the vertex shader
    float glow = clamp(glowStrength, 0.0, 1.0);

    // Combine base color with glow as alpha
    finalColor = vec4(baseColor, glow) * fragColor;
}
