#version 330

uniform vec3 baseColor;

in vec3 fragNormal;
out vec4 finalColor;

void main() {
    vec3 normal = normalize(fragNormal);

    float ambient = 0.4;
    float diffuse = max(dot(normal, vec3(0.0, 0.0, 1.0)), 0.0); // directional light from Z+
    float lighting = ambient + diffuse * 0.6; // total will be between 0.4 and 1.0

    vec3 color = baseColor * lighting;
    finalColor = vec4(color, 1.0);
}
