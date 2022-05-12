#pragma once

#include "shader.h"

#include <memory>

namespace gp {
	
	class Rect {
		
	public:
		
		Rect(const std::shared_ptr <Shader>& _shader) noexcept;
		
		~Rect() noexcept;
		
		void render() noexcept;
		
	private:
		
		std::shared_ptr <Shader> m_shader;
		
		unsigned int m_vao, m_vbo, m_ebo;
		
	};
	
} /// namespace gp
