precision highp float;

uniform float time;

varying vec2 fUv;
varying vec3 fNormal;

void main() {
    float t = time / 1000.;
    float r = fUv.y;
    float g = (cos(t) * 0.25 + 0.5);
    gl_FragColor = vec4(r, g * abs(dot(fNormal, vec3(0.0, 0.0, 1.0))), pow(cos(t), 1.5) * pow(sin(t * 1.1), 0.9), 1.);
}
