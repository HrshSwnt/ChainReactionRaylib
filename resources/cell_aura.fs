#version 330

uniform vec3 baseColor;
uniform float time;

in vec3 fragNormal;
in vec4 fragPosition;

out vec4 finalColor;

void main() {
    float glow = 0.5 + 0.5 * sin(time * 3.0); // Pulsing intensity

    // Edge fade for glowiness at edges
    float edgeFade = pow(1.0 - abs(dot(normalize(fragNormal), vec3(0.0, 0.0, 1.0))), 2.0);

    // Depth-based fading
    float depth = fragPosition.z / fragPosition.w;           // Convert to normalized depth
    float fade = smoothstep(0.0, 1.0, depth);                 // Near = 0 → Far = 1

    vec3 auraColor = baseColor * glow;
    float alpha = 0.5 * edgeFade * fade;

    finalColor = vec4(auraColor, alpha);
}
