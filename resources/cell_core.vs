#version 330

uniform mat4 mvp;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 fragNormal;

void main() {
    fragNormal = vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
