#include "rect.h"

#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gp {
	
	extern int framebuffer_height;
	extern int framebuffer_width;
	
	Rect::Rect(const std::shared_ptr <Shader>& _shader) noexcept :
	m_shader(_shader),
	m_vao(0),
	m_vbo(0),
	m_ebo(0)
	{
		static const float_t vertices[2 * 4] = {
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,  1.0f
		};
		static const uint32_t indices[3 * 2] = {
			0, 1, 2,
			1, 3, 2
		};
		
		_shader->bind();
		
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);
		
		glBindVertexArray(m_vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * 2 * 4, vertices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3 * 2, indices, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 2, (void*) 0);
		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		_shader->unbind();
	}
	
	Rect::~Rect() noexcept {
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ebo);
	}
	
	void Rect::render() noexcept {
		m_shader->bind();
		glBindVertexArray(m_vao);
		
		glm::mat4 proj = glm::ortho(
		-float(framebuffer_width) * 0.5f, float(framebuffer_width) * 0.5f,
		-float(framebuffer_height) * 0.5f, float(framebuffer_height) * 0.5f,
		-1.0f, 1.0f
		);
		m_shader->set_uniform_mat4("u_proj", proj);
		
		glm::mat4 model(1.0f);
		model = glm::scale(model,
		glm::vec3(float(framebuffer_width) * 0.5f, float(framebuffer_height) * 0.5f, 1.0f));
		m_shader->set_uniform_mat4("u_model", model);
		
		m_shader->set_uniform_1f("u_height", framebuffer_height);
		m_shader->set_uniform_1f("u_width", framebuffer_width);
		
		glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, (void*) 0);
		
		glBindVertexArray(0);
		m_shader->unbind();
	}
	
} /// namespace gp
