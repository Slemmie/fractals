#version 460 core

layout (location = 0) in vec2 pos;

uniform mat4 u_proj;
uniform mat4 u_model;

out vec2 frag_pos;

uniform float u_width;
uniform float u_height;

uniform float u_offset_x;
uniform float u_offset_y;

void main() {
	gl_Position = u_proj * u_model * vec4(pos, 0.0f, 1.0f);
	
	frag_pos = (u_model * vec4(pos * vec2(1.0f, -1.0f), 0.0f, 1.0f)).xy +
	vec2(u_width, u_height) * 0.5f + vec2(u_offset_x, u_offset_y);
}
