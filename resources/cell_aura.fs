// GLSL ES 1.0 fragment shader

precision mediump float;

uniform vec3 baseColor;
uniform float time;

varying vec3 fragNormal;
varying vec4 fragPosition;

void main() {
    float glow = 0.5 + 0.5 * sin(time * 3.0);

    float edgeFade = pow(1.0 - abs(dot(normalize(fragNormal), vec3(0.0, 0.0, 1.0))), 2.0);

    float depth = fragPosition.z / fragPosition.w;
    float fade = smoothstep(0.0, 1.0, depth);

    vec3 auraColor = baseColor * glow;
    float alpha = 0.5 * edgeFade * fade;

    gl_FragColor = vec4(auraColor, alpha);
}
