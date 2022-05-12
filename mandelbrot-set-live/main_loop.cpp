#include "main_loop.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

namespace gp {
	
	extern GLFWwindow* window_handle;
	
	void launch_main_loop(const std::function <void ()>& frame_callback) noexcept {
		while (!glfwWindowShouldClose(window_handle)) {
			glClear(GL_COLOR_BUFFER_BIT);
			
			frame_callback();
			
			glfwSwapBuffers(window_handle);
			
			glfwPollEvents();
		}
	}
	
} /// namespace gp
