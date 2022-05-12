#include "gp.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <thread>
#include <shared_mutex>
#include <iostream>

namespace gp {
	
	GLFWwindow* window_handle = nullptr;
	int window_width = 1280, window_height = 760;
	int framebuffer_width, framebuffer_height;
	
	bool is_initialized = false;
	bool is_terminated = false;
	std::thread::id main_thread_id;
	
	std::shared_mutex mutex;
	
	double scroll_delta = 0;
	double total_scroll = 1;
	
	void init() noexcept {
		std::unique_lock lock(mutex);
		
		if (is_initialized || is_terminated) {
			std::cerr << "[warning]: illegal call to gp::init()" << std::endl;
			return;
		}
		is_initialized = true;
		
		main_thread_id = std::this_thread::get_id();
		
		if (!glfwInit()) {
			std::cerr << "[fatal]: failed to initalize glfw" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		glfwSetErrorCallback([] (int error_id, const char* error_message) -> void {
			std::cerr << "[glfw error (#" << error_id << ")]: " << error_message << std::endl;
		});
		
		if (!(window_handle = glfwCreateWindow(window_width, window_height, "", nullptr, nullptr))) {
			std::cerr << "[fatal]: failed to create window" << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		
		glfwMakeContextCurrent(window_handle);
		
		if (glewInit() != GLEW_OK) {
			std::cerr << "[fatal]: failed to initalize glew" << std::endl;
			glfwDestroyWindow(window_handle);
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		
		glfwGetFramebufferSize(window_handle, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glfwSwapInterval(0);
		
		glfwSetFramebufferSizeCallback(window_handle,
		[] (GLFWwindow* w_handle, int width, int height) -> void {
			std::unique_lock local_lock(mutex);
			
			glViewport(0, 0, framebuffer_width = width, framebuffer_height = height);
			glfwGetWindowSize(w_handle, &window_width, &window_height);
		});
		
		glfwSetScrollCallback(window_handle,
		[] (GLFWwindow* w_handle, double x_offset, double y_offset) -> void {
			std::unique_lock local_lock(mutex);
			
			double delta = y_offset * total_scroll * 0.1;
			
			total_scroll -= delta;
			
			scroll_delta -= delta;
		});
	}
	
	void terminate() noexcept {
		std::unique_lock lock(mutex);
		
		if (!is_initialized || is_terminated) {
			std::cerr << "[warning]: illegal call to gp::terminate()" << std::endl;
			return;
		}
		is_terminated = true;
		
		if (std::this_thread::get_id() != main_thread_id) {
			std::cerr << "[warning]: gp::init()/gp::terminate() thread mismatch" << std::endl;
			return;
		}
		
		if (window_handle) {
			glfwDestroyWindow(window_handle);
		}
		
		glfwTerminate();
	}
	
	void set_window_title(const std::string& title) noexcept {
		std::unique_lock lock(mutex);
		
		glfwSetWindowTitle(window_handle, title.c_str());
	}
	
	bool is_pressed() noexcept {
		return glfwGetMouseButton(window_handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	}
	
	double mouse_x() noexcept {
		double x, y;
		glfwGetCursorPos(window_handle, &x, &y);
		return x;
	}
	
	double mouse_y() noexcept {
		double x, y;
		glfwGetCursorPos(window_handle, &x, &y);
		return y;
	}
	
	double pop_scroll_delta() noexcept {
		double result = scroll_delta;
		scroll_delta = 0;
		return result;
	}
	
	double get_total_scroll() noexcept {
		return total_scroll;
	}
	
} /// namespace gp
