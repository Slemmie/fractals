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

double lerp(double a, double b, double t) {
	return a + t * (b - a);
}

dvec2 iterate(const dvec2 z, const dvec2 c) {
	return dvec2(z.x * z.x - z.y * z.y + c.x, z.x * z.y * 2.0 + c.y);
}

double mandelbrot(const dvec2 c) {
	int count = 0;
	dvec2 z = dvec2(0, 0);
	do {
		if (count++ == u_max_iterations) {
			return u_max_iterations;
		}
		z = iterate(z, c);
	} while (z.x * z.x + z.y * z.y < 4.0);
	return double(count) + 1.0 - double(log(log2(float(sqrt(z.x * z.x + z.y * z.y)))));
}

double get_iterations(const double x, const double y) {
	const double c_r = lerp(u_r_begin, u_r_end, x / u_width);
	const double c_i = lerp(u_i_begin, u_i_end, y / u_height);
	return mandelbrot(dvec2(c_r, c_i));
}

const dvec4 gradient[] = {
	dvec4(185.0 / 255.0, 210.0 / 255.0, 241.0 / 255.0, 0.00),
	dvec4(185.0 / 255.0, 101.0 / 255.0,  35.0 / 255.0, 0.08),
	dvec4(213.0 / 255.0,  19.0 / 255.0,  88.0 / 255.0, 0.15),
	dvec4(226.0 / 255.0, 141.0 / 255.0, 193.0 / 255.0, 0.26),
	dvec4( 69.0 / 255.0,  45.0 / 255.0, 176.0 / 255.0, 0.44),
	dvec4( 75.0 / 255.0,  92.0 / 255.0, 218.0 / 255.0, 0.70),
	dvec4(185.0 / 255.0, 210.0 / 255.0, 241.0 / 255.0, 1.00)
};
const double gradient_scale = 0.009;

dvec3 pixel(const double iterations) {
	if (int(ceil(iterations)) >= u_max_iterations) {
		return dvec3(0.0);
	}
	
	const double gradient_pos = iterations * gradient_scale;
	const double gradient_pos_normalized = gradient_pos - floor(gradient_pos);
	
	int index = 0;
	while (gradient[index + 1].w < gradient_pos_normalized) {
		index++;
	}
	double t = (gradient_pos_normalized - gradient[index].w) / (gradient[index + 1].w - gradient[index].w);
	
	return dvec3(
	lerp(gradient[index].r, gradient[index + 1].r, t),
	lerp(gradient[index].g, gradient[index + 1].g, t),
	lerp(gradient[index].b, gradient[index + 1].b, t)
	);
}

void main() {
	frag_color = vec4(1.0f);
	
	frag_color.rgb = vec3(pixel(get_iterations(frag_pos.x, frag_pos.y)));
}
