#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec2 fragTexCoord;
out vec4 fragColor;
out float glowStrength;

void main() {
    // Transform vertex position
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    // Pass through texture coordinate and color
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate normal in world space
    vec3 normal = normalize(mat3(model) * vertexNormal);

    // Calculate light direction and view direction
    vec3 fragPos = vec3(model * vec4(vertexPosition, 1.0));
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);

    // Simple diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Simple glow: stronger at the edges (facing away from view)
    glowStrength = pow(1.0 - max(dot(normal, viewDir), 0.0), 2.0) * 0.8 + diff * 0.2;
}
