// GLSL ES 1.0 Vertex Shader

precision mediump float;

uniform mat4 mvp;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

varying vec3 fragNormal;

void main() {
    fragNormal = vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
