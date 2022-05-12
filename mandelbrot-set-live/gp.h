#pragma once

#include "shader.h"
#include "main_loop.h"
#include "rect.h"

#include <string>

namespace gp {
	
	void init() noexcept;
	
	void terminate() noexcept;
	
	void set_window_title(const std::string& title) noexcept;
	
} /// namespace gp
