#pragma once

#include "shader.h"
#include "main_loop.h"
#include "rect.h"

#include <string>

namespace gp {
	
	void init() noexcept;
	
	void terminate() noexcept;
	
	void set_window_title(const std::string& title) noexcept;
	
	bool is_pressed() noexcept;
	double mouse_x() noexcept;
	double mouse_y() noexcept;
	
	double pop_scroll_delta() noexcept;
	double get_total_scroll() noexcept;
	
} /// namespace gp
