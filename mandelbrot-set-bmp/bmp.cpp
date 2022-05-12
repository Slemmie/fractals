#include "bmp.h"

#include <cassert>
#include <cstring>

namespace BMP {
	
	constexpr const size_t FILE_HEADER_SIZE = 14;
	constexpr const size_t INFO_HEADER_SIZE = 40;
	constexpr const size_t BYTES_PER_PIXEL = 3;
	
	void append_header(FILE* file, const size_t file_size) noexcept;
	void append_info_header(FILE* file, const size_t height, const size_t width) noexcept;
	void append_pixels(FILE* file, uint8_t* buffer, const size_t height, const size_t width) noexcept;
	
	void save(const std::string& filepath, const uint8_t* buffer,
	const size_t height, const size_t width, const size_t stride) noexcept {
		uint8_t* local_buffer = new uint8_t[height * width * BYTES_PER_PIXEL];
		assert(local_buffer);
		for (size_t i = 0; i < height; i++) {
			for (size_t j = 0; j < width; j++) {
				local_buffer[((height - i - 1) * width + j) * 3 + 0] = buffer[(i * width + j) * 3 + 2];
				local_buffer[((height - i - 1) * width + j) * 3 + 1] = buffer[(i * width + j) * 3 + 1];
				local_buffer[((height - i - 1) * width + j) * 3 + 2] = buffer[(i * width + j) * 3 + 0];
			}
		}
		FILE* file = fopen(filepath.c_str(), "wb");
		assert(file);
		append_header(file, stride * height + FILE_HEADER_SIZE + INFO_HEADER_SIZE);
		append_info_header(file, height, width);
		append_pixels(file, local_buffer, height, width);
		assert(!fclose(file));
		delete [] (local_buffer);
	}
	
	void append_header(FILE* file, const size_t file_size) noexcept {
		static uint8_t header[] = {
			0, 0,       // signature
			0, 0, 0, 0, // file size in bytes
			0, 0, 0, 0, // reserved
			0, 0, 0, 0  // start of pixel buffer
		};
		
		header[0] = (uint8_t) ('B');
		header[1] = (uint8_t) ('M');
		header[2] = (uint8_t) (file_size >>  0);
		header[3] = (uint8_t) (file_size >>  8);
		header[4] = (uint8_t) (file_size >> 16);
		header[5] = (uint8_t) (file_size >> 24);
		header[10] = (uint8_t) (FILE_HEADER_SIZE + INFO_HEADER_SIZE);
		
		fwrite(header, 1, FILE_HEADER_SIZE, file);
	}
	
	void append_info_header(FILE* file, const size_t height, const size_t width) noexcept {
		static uint8_t header[] = {
			0, 0, 0, 0, // header size
			0, 0, 0, 0, // image width
			0, 0, 0, 0, // image heght
			0, 0,       // number of color planes
			0, 0,       // bits per pixel
			0, 0, 0, 0, // compression
			0, 0, 0, 0, // image size
			0, 0, 0, 0, // horizontal resolution
			0, 0, 0, 0, // vertical resolution
			0, 0, 0, 0, // colors in color table
			0, 0, 0, 0, // important color count
		};
		
		header[0] = (uint8_t) (INFO_HEADER_SIZE);
		header[4] = (uint8_t) (width >> 0);
		header[5] = (uint8_t) (width >> 8);
		header[6] = (uint8_t) (width >> 16);
		header[7] = (uint8_t) (width >> 24);
		header[8] = (uint8_t) (height >> 0);
		header[9] = (uint8_t) (height >> 8);
		header[10] = (uint8_t) (height >> 16);
		header[11] = (uint8_t) (height >> 24);
		header[12] = (uint8_t) (1);
		header[14] = (uint8_t) (BYTES_PER_PIXEL * 8);
		
		fwrite(header, 1, INFO_HEADER_SIZE, file);
	}
	
	void append_pixels(FILE* file, uint8_t* buffer, const size_t height, const size_t width) noexcept {
		size_t width_bytes = width * BYTES_PER_PIXEL;
		uint8_t padding[3] = { 0, 0, 0 };
		size_t padding_size = (4 - width_bytes % 4) % 4;
		uint8_t* data = &buffer[0];
		for (size_t i = 0; i < height; i++) {
			fwrite(data + (i * width_bytes), BYTES_PER_PIXEL, width, file);
			fwrite(padding, 1, padding_size, file);
		}
	}
	
} /// namespace BMP
