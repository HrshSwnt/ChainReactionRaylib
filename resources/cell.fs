#version 330

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 finalColor;

uniform float time;
uniform vec3 baseColor;

void main() {
    // Diffuse lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(fragNormal), lightDir), 0.0);

    // Aura glow (pulsating only in falloff area)
    float distance = length(fragPosition);
    float outerFalloff = smoothstep(0.5, 1.2, distance);  // only outer edge glows
    float glowPulse = 0.4 + 0.3 * sin(time * 2.0 + distance * 5.0);
    vec3 glow = baseColor * glowPulse * outerFalloff;

    // Combine diffuse surface and glow aura
    vec3 resultColor = baseColor * diff + glow;
    finalColor = vec4(resultColor, 1.0);
}
