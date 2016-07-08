#version 430
layout(location = 0) in vec3 Position;

out vec2 outTexCoord;

void main() {
	outTexCoord = Position.xy * 0.5 + vec2(0.5);
	gl_Position = vec4(Position, 1.0);
}