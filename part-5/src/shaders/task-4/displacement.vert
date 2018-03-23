attribute vec3 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform sampler2D textureDisplacement;
uniform vec2 textureDimension;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    float heightScaling = 0.8;
    float normalScaling = 1.0;
    float height = texture2D(textureDisplacement, uv).x;
    mat3 TBN = mat3(tangent, cross(normal, tangent), normal);
    
    float du = texture2D(textureDisplacement, vec2(uv.x + 1.0 / textureDimension.x, uv.y)).x - height;
    du *= heightScaling * normalScaling;
    float dv = texture2D(textureDisplacement, vec2(uv.x, uv.y + 1.0 / textureDimension.y)).x - height;
    dv *= heightScaling * normalScaling;
    fNormal = TBN * vec3(-du, -dv, 1.0);
    vec3 offset = position + fNormal * height * heightScaling;
    fPosition = offset;
    
    gl_Position = projectionMatrix * modelViewMatrix * vec4(offset, 1.0);
}
