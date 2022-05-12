#version 460 core

uniform float u_width;
uniform float u_height;

uniform int u_max_iterations;

uniform float u_zoom;
uniform float u_zoom_x;
uniform float u_zoom_y;

uniform float u_r_begin;
uniform float u_r_end;
uniform float u_i_begin;
uniform float u_i_end;

in vec2 frag_pos;

out vec4 frag_color;

float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

vec2 iterate(const vec2 z, const vec2 c) {
	return vec2(z.x * z.x - z.y * z.y + c.x, z.x * z.y * 2.0f + c.y);
}

int mandelbrot(const vec2 c) {
	int count = 0;
	vec2 z = vec2(0, 0);
	do {
		if (count++ == u_max_iterations) {
			return u_max_iterations;
		}
		z = iterate(z, c);
	} while (z.x * z.x + z.y * z.y < 4.0f);
	return count;
}

int get_iterations(const float x, const float y) {
	const float c_r = lerp(u_r_begin, u_r_end, x / u_width);
	const float c_i = lerp(u_i_begin, u_i_end, y / u_height);
	return mandelbrot(vec2(c_r, c_i));
}

const vec4 gradient[] = {
	vec4(185.0f / 255.0f, 210.0f / 255.0f, 241.0f / 255.0f, 0.00f),
	vec4(185.0f / 255.0f, 101.0f / 255.0f,  35.0f / 255.0f, 0.08f),
	vec4(213.0f / 255.0f,  19.0f / 255.0f,  88.0f / 255.0f, 0.15f),
	vec4(226.0f / 255.0f, 141.0f / 255.0f, 193.0f / 255.0f, 0.26f),
	vec4( 69.0f / 255.0f,  45.0f / 255.0f, 176.0f / 255.0f, 0.44f),
	vec4( 75.0f / 255.0f,  92.0f / 255.0f, 218.0f / 255.0f, 0.70f),
	vec4(185.0f / 255.0f, 210.0f / 255.0f, 241.0f / 255.0f, 1.00f)
};
const float gradient_scale = 0.009f;

vec3 pixel(const int iterations) {
	if (iterations >= u_max_iterations) {
		return vec3(0.0f);
	}
	
	const float gradient_pos = float(iterations) * gradient_scale;
	const float gradient_pos_normalized = gradient_pos - floor(gradient_pos);
	
	int index = 0;
	while (gradient[index + 1].w < gradient_pos_normalized) {
		index++;
	}
	float t = (gradient_pos_normalized - gradient[index].w) / (gradient[index + 1].w - gradient[index].w);
	
	return vec3(
	lerp(gradient[index].r, gradient[index + 1].r, t),
	lerp(gradient[index].g, gradient[index + 1].g, t),
	lerp(gradient[index].b, gradient[index + 1].b, t)
	);
}

void main() {
	frag_color = vec4(1.0f);
	
	frag_color.rgb = pixel(get_iterations(frag_pos.x, frag_pos.y));
}
