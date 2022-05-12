#pragma once

#include <functional>

namespace gp {
	
	void launch_main_loop(const std::function <void ()>& frame_callback) noexcept;
	
} /// namespace gp
