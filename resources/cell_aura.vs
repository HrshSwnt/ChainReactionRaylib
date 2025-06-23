// cell_aura.vs - GLSL ES 1.0

precision mediump float;

uniform mat4 mvp;
uniform float time;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

varying vec3 fragNormal;
varying vec4 fragPosition; // Pass to FS for depth-based fade

void main() {
    float pulseScale = 1.2 + 0.1 * sin(time * 3.0);
    vec3 scaledPosition = vertexPosition * pulseScale;

    fragNormal = vertexNormal;
    fragPosition = mvp * vec4(scaledPosition, 1.0); // Save clip-space position
    gl_Position = fragPosition;
}
