precision highp float;

uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.1
    float len = length(lPosition - fPosition);
    gl_FragColor = vec4(lIntensity / (len * len) * max(0.0, dot(normalize(lPosition), fNormal)) , 1.0);
}
