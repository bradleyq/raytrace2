precision highp float;

uniform vec3 cameraPosition;
uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.2
    float kd = 0.5;
    float ks = 0.6;
    float p = 30.0;
    vec3 ambient = vec3(0.1, 0.1, 0.1);
    float len = length(lPosition - fPosition);
    vec3 diffuse = kd * lIntensity / (len * len) * max(0.0, dot(normalize(lPosition), fNormal));
    vec3 specular = ks * lIntensity / (len * len) * pow(max(0.0, dot(fNormal, normalize(normalize(lPosition - cameraPosition) + normalize(cameraPosition - fPosition)))), p);
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
