#pragma once

#include <string>

namespace BMP {
	
	void save(const std::string& filepath, const uint8_t* buffer,
	const size_t height, const size_t width, const size_t stride) noexcept ;
	
} /// namespace BMP
