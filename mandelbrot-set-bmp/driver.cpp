#include "bmp.h"

#include <bits/stdc++.h>

constexpr const size_t WIDTH = static_cast <size_t> (1024);
constexpr const size_t HEIGHT = static_cast <size_t> (1024);
//constexpr const size_t WIDTH = static_cast <size_t> (4096);
//constexpr const size_t HEIGHT = static_cast <size_t> (4096);

constexpr const size_t MAX_ITERATIONS = static_cast <size_t> (200'000);

constexpr double lerp(double a, double b, double t) noexcept {
	return a + t * (b - a);
}

constexpr std::complex <double> iterate(const std::complex <double> z, const std::complex <double> c) noexcept {
	return std::complex <double> (
	z.real() * z.real() - z.imag() * z.imag() + c.real(),
	z.real() * z.imag() * 2.0 + c.imag());
}

inline size_t mandelbrot(const std::complex <double> c) noexcept {
	size_t count = 0;
	std::complex <double> z(0, 0);
	do {
		if (count++ == MAX_ITERATIONS) {
			return MAX_ITERATIONS;
		}
		z = iterate(z, c);
	} while (z.real() * z.real() + z.imag() * z.imag() < 4.0);
	return count;
}

struct Gradient_entry {
	double r, g, b, s;
};

inline void pixel(uint8_t* buffer, const size_t iterations) noexcept {
	static const Gradient_entry gradient[] = {
		{ 185, 210, 241, 0.00 },
		{ 185, 101,  35, 0.08 },
		{ 213,  19,  88, 0.15 },
		{ 226, 141, 193, 0.26 },
		{  69,  45, 176, 0.44 },
		{  75,  92, 218, 0.70 },
		{ 185, 210, 241, 1.00 }
	};
	
	static const double gradient_scale = 0.009;
	
	if (iterations >= MAX_ITERATIONS) {
		buffer[0] = buffer[1] = buffer[2] = 0;
		return;
	}
	
	const double gradient_pos = (double) iterations * gradient_scale;
	const double gradient_pos_normalized = gradient_pos - floor(gradient_pos);
	
	size_t index = 0;
	while (gradient[index + 1].s < gradient_pos_normalized) {
		index++;
	}
	double t =
	(gradient_pos_normalized - gradient[index].s) /
	(gradient[index + 1].s - gradient[index].s);
	
	buffer[0] = uint8_t(lerp(gradient[index].r, gradient[index + 1].r, t));
	buffer[1] = uint8_t(lerp(gradient[index].g, gradient[index + 1].g, t));
	buffer[2] = uint8_t(lerp(gradient[index].b, gradient[index + 1].b, t));
}

constexpr bool compare(const uint8_t* u, const uint8_t* v) noexcept {
	return
	(((uint32_t) u[0] << 16) | ((uint32_t) u[1] << 8) | u[2])
	==
	(((uint32_t) v[0] << 16) | ((uint32_t) v[1] << 8) | v[2]);
}

void compute(uint8_t* buffer,
const size_t left, const size_t right, const size_t top, const size_t bottom,
const double zoom, const double zoom_x, const double zoom_y) noexcept {
	assert(left < right && top < bottom);
	assert(right - left == WIDTH && bottom - top == HEIGHT);
	
	const double r_begin = -2.0  / zoom + zoom_x;
	const double r_end = 2.0 / zoom + zoom_x;
	const double i_begin = -2.0 / zoom - zoom_y;
	const double i_end = 2.0 / zoom - zoom_y;
	
	auto calculate = [&] (const size_t x, const size_t y) -> size_t {
		const double c_r = lerp(r_begin, r_end, (double) x / WIDTH);
		const double c_i = lerp(i_begin, i_end, (double) y / HEIGHT);
		return mandelbrot(std::complex <double> (c_r, c_i));
	};
	
	const size_t thread_count = std::thread::hardware_concurrency();
	const size_t square_size = WIDTH * HEIGHT / 1024 <= 2048 ? 8 : 16;
	
	std::mutex mutex;
	size_t current_x = 0;
	size_t current_y = 0;
	
	auto job = [&] (const size_t index) -> void {
		size_t sx = 0, sy = 0;
		while (true) {
			{
				std::unique_lock <std::mutex> lock(mutex);
				sx = current_x;
				sy = current_y;
				if ((current_x += square_size) >= WIDTH) {
					current_x = 0;
					current_y += square_size;
				}
			}
			
			if (sx >= WIDTH || sy >= HEIGHT) {
				break;
			}
			
			size_t ex = std::min(sx + square_size, right);
			size_t ey = std::min(sy + square_size, bottom);
			
			bool same = true;
			
			for (size_t y = sy; y < ey; y++) {
				pixel(&buffer[(y * WIDTH + sx) * 3], calculate(sx + left, y + top));
				pixel(&buffer[(y * WIDTH + (ex - 1)) * 3], calculate(ex - 1 + left, y + top));
				
				same &= compare(
				&buffer[(y * WIDTH + sx) * 3],
				&buffer[(y * WIDTH + (ex - 1)) * 3]
				);
				
				same &= compare(
				&buffer[(sy * WIDTH + sx) * 3],
				&buffer[(y * WIDTH + sx) * 3]
				);
			}
			for (size_t x = sx + 1; x < ex - 1; x++) {
				pixel(&buffer[(sy * WIDTH + x) * 3], calculate(x + left, sy + top));
				pixel(&buffer[((ey - 1) * WIDTH + x) * 3], calculate(x + left, ey - 1 + top));
				
				same &= compare(
				&buffer[(sy * WIDTH + x) * 3],
				&buffer[((ey - 1) * WIDTH + x) * 3]
				);
				
				same &= compare(
				&buffer[(sy * WIDTH + sx) * 3],
				&buffer[(sy * WIDTH + x) * 3]
				);
			}
			
			if (same) {
				uint8_t r = buffer[(sy * WIDTH + sx) * 3 + 0];
				uint8_t g = buffer[(sy * WIDTH + sx) * 3 + 1];
				uint8_t b = buffer[(sy * WIDTH + sx) * 3 + 2];
				for (size_t y = sy + 1; y < ey - 1; y++) {
					for (size_t x = sx + 1; x < ex - 1; x++) {
						buffer[(y * WIDTH + x) * 3 + 0] = r;
						buffer[(y * WIDTH + x) * 3 + 1] = g;
						buffer[(y * WIDTH + x) * 3 + 2] = b;
					}
				}
				continue;
			}
			
			for (size_t y = sy + 1; y < ey - 1; y++) {
				for (size_t x = sx + 1; x < ex - 1; x++) {
					pixel(&buffer[(y * WIDTH + x) * 3], calculate(x + left, y + top));
				}
			}
		}
	};
	
	std::vector <std::thread> threads;
	for (size_t index = 0; index < thread_count; index++) {
		threads.emplace_back(job, index);
	}
	for (std::thread& thread : threads) {
		thread.join();
	}
}

inline bool equal_perim(uint8_t* buffer,
const size_t sx, const size_t sy, const size_t ex, const size_t ey) noexcept {
	uint8_t r = buffer[(sy * WIDTH + sx) * 3 + 0];
	uint8_t g = buffer[(sy * WIDTH + sx) * 3 + 1];
	uint8_t b = buffer[(sy * WIDTH + sx) * 3 + 2];
	uint32_t rgb = ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
	uint8_t* u = nullptr;
	for (size_t y = sy; y <= ey; y++) {
		u = &buffer[(y * WIDTH + sx) * 3];
		if ((((uint32_t) u[0] << 16) | ((uint32_t) u[1] << 8) | u[2]) != rgb) {
			return false;
		}
		u = &buffer[(y * WIDTH + ex) * 3];
		if ((((uint32_t) u[0] << 16) | ((uint32_t) u[1] << 8) | u[2]) != rgb) {
			return false;
		}
	}
	for (size_t x = sx; x <= ex; x++) {
		u = &buffer[(sy * WIDTH + x) * 3];
		if ((((uint32_t) u[0] << 16) | ((uint32_t) u[1] << 8) | u[2]) != rgb) {
			return false;
		}
		u = &buffer[(ey * WIDTH + x) * 3];
		if ((((uint32_t) u[0] << 16) | ((uint32_t) u[1] << 8) | u[2]) != rgb) {
			return false;
		}
	}
	return true;
}

void compute_quad(uint8_t* buffer,
const size_t left, const size_t right, const size_t top, const size_t bottom,
const double zoom, const double zoom_x, const double zoom_y) noexcept {
	assert(left < right && top < bottom);
	assert(right - left == WIDTH && bottom - top == HEIGHT);
	
	const double r_begin = -2.0  / zoom + zoom_x;
	const double r_end = 2.0 / zoom + zoom_x;
	const double i_begin = -2.0 / zoom - zoom_y;
	const double i_end = 2.0 / zoom - zoom_y;
	
	auto calculate = [&] (const size_t x, const size_t y) -> size_t {
		const double c_r = lerp(r_begin, r_end, (double) x / WIDTH);
		const double c_i = lerp(i_begin, i_end, (double) y / HEIGHT);
		return mandelbrot(std::complex <double> (c_r, c_i));
	};
	
	const size_t thread_count = std::thread::hardware_concurrency();
	const size_t square_size = WIDTH * HEIGHT / 1024 <= 2048 ? 8 : 16;
	
	std::mutex mutex;
	size_t current_x = 0;
	size_t current_y = 0;
	
	auto quad_job = [&] (auto&& self,
	const size_t sx, const size_t sy, const size_t ex, const size_t ey) -> void {
		if (std::min(ex - sx, ey - sy) < static_cast <size_t> (5)) {
			for (size_t y = sy + 1; y < ey - 1; y++) {
				for (size_t x = sx + 1; x < ex - 1; x++) {
					pixel(&buffer[(y * WIDTH + x) * 3], calculate(x + left, y + top));
				}
			}
			return;
		}
		
		const size_t mx = (sx + ex) >> 1;
		const size_t my = (sy + ey) >> 1;
		
		for (size_t y = sy + 1; y < ey - 1; y++) {
			pixel(&buffer[(y * WIDTH + mx) * 3], calculate(mx + left, y + top));
		}
		for (size_t x = sx + 1; x < ex - 1; x++) {
			pixel(&buffer[(my * WIDTH + x) * 3], calculate(x + left, my + top));
		}
		
		uint8_t r = buffer[(my * WIDTH + mx) * 3 + 0];
		uint8_t g = buffer[(my * WIDTH + mx) * 3 + 1];
		uint8_t b = buffer[(my * WIDTH + mx) * 3 + 2];
		
		if (equal_perim(buffer, sx, sy, mx, my)) {
			for (size_t y = sy + 1; y < my; y++) {
				for (size_t x = sx + 1; x < mx; x++) {
					buffer[(y * WIDTH + x) * 3 + 0] = r;
					buffer[(y * WIDTH + x) * 3 + 1] = g;
					buffer[(y * WIDTH + x) * 3 + 2] = b;
				}
			}
		} else {
			self(self, sx, sy, mx + 1, my + 1);
		}
		
		if (equal_perim(buffer, sx, my, mx, ey - 1)) {
			for (size_t y = my + 1; y < ey - 1; y++) {
				for (size_t x = sx + 1; x < mx; x++) {
					buffer[(y * WIDTH + x) * 3 + 0] = r;
					buffer[(y * WIDTH + x) * 3 + 1] = g;
					buffer[(y * WIDTH + x) * 3 + 2] = b;
				}
			}
		} else {
			self(self, sx, my, mx + 1, ey);
		}
		
		if (equal_perim(buffer, mx, sy, ex - 1, my)) {
			for (size_t y = sy + 1; y < my; y++) {
				for (size_t x = mx + 1; x < ex - 1; x++) {
					buffer[(y * WIDTH + x) * 3 + 0] = r;
					buffer[(y * WIDTH + x) * 3 + 1] = g;
					buffer[(y * WIDTH + x) * 3 + 2] = b;
				}
			}
		} else {
			self(self, mx, sy, ex, my + 1);
		}
		
		if (equal_perim(buffer, mx, my, ex - 1, ey - 1)) {
			for (size_t y = my + 1; y < ey - 1; y++) {
				for (size_t x = mx + 1; x < ex - 1; x++) {
					buffer[(y * WIDTH + x) * 3 + 0] = r;
					buffer[(y * WIDTH + x) * 3 + 1] = g;
					buffer[(y * WIDTH + x) * 3 + 2] = b;
				}
			}
		} else {
			self(self, mx, my, ex, ey);
		}
	};
	
	auto job = [&] (const size_t index) -> void {
		size_t sx = 0, sy = 0;
		while (true) {
			{
				std::unique_lock <std::mutex> lock(mutex);
				sx = current_x;
				sy = current_y;
				if ((current_x += square_size) >= WIDTH) {
					current_x = 0;
					current_y += square_size;
				}
			}
			
			if (sx >= WIDTH || sy >= HEIGHT) {
				break;
			}
			
			size_t ex = std::min(sx + square_size, right);
			size_t ey = std::min(sy + square_size, bottom);
			
			for (size_t y = sy; y < ey; y++) {
				pixel(&buffer[(y * WIDTH + sx) * 3], calculate(sx + left, y + top));
				pixel(&buffer[(y * WIDTH + (ex - 1)) * 3], calculate((ex - 1) + left, y + top));
			}
			for (size_t x = sx + 1; x < ex - 1; x++) {
				pixel(&buffer[(sy * WIDTH + x) * 3], calculate(x + left, sy + top));
				pixel(&buffer[((ey - 1) * WIDTH + x) * 3], calculate(x + left, (ey - 1) + top));
			}
			
			quad_job(quad_job, sx, sy, ex, ey);
		}
	};
	
	std::vector <std::thread> threads;
	for (size_t index = 0; index < thread_count; index++) {
		threads.emplace_back(job, index);
	}
	for (std::thread& thread : threads) {
		thread.join();
	}
}

int main() {
	
	uint8_t buffer[WIDTH * HEIGHT * 3];
	
	const double zoom = 6.290223e+3;
	const double zoom_x = -1.186592e+0;
	const double zoom_y = -1.901211e-1;
	
	std::cerr << "calculating mandelbrot set..." << std::endl;
	const auto start = std::chrono::high_resolution_clock::now();
	
	compute_quad(buffer, 0, WIDTH, 0, HEIGHT, zoom, zoom_x, zoom_y); // ~39 ms
	//compute(buffer, 0, WIDTH, 0, HEIGHT, zoom, zoom_x, zoom_y); // ~42 ms
	
	const auto finish = std::chrono::high_resolution_clock::now();
	const auto millis = std::chrono::duration_cast <std::chrono::milliseconds> (finish - start);
	std::cerr << "completed in " << millis.count() << " ms" << std::endl;
	
	std::cerr << "saving to 'out.bmp'..." << std::endl;
	BMP::save("out.bmp", buffer, HEIGHT, WIDTH, WIDTH);
	
	std::cerr << "done" << std::endl;
	
}
