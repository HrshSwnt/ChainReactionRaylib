#version 330

// Default vertex attributes
in vec3 vertexPosition;
in vec3 vertexNormal;

// Output to fragment shader
out vec3 fragNormal;
out vec3 fragPosition;

// MVP matrix from Raylib
uniform mat4 mvp;

void main() {
    fragNormal = normalize(vertexNormal);
    fragPosition = vertexPosition;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
