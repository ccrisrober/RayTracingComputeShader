#version 430

in vec2 outTexCoord;

layout(binding=0) uniform sampler2D DiffSpecTex;
layout( location = 0 ) out vec4 FragColor;

void main() {
    vec4 diffSpec = texture(DiffSpecTex, outTexCoord);
    FragColor = vec4(diffSpec.xyz, 1);
}