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
	
} /// namespace gp
