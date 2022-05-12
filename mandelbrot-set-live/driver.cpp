#include "gp.h"

#include <memory>

int main() {
	gp::init();
	
	{
		std::shared_ptr <gp::Shader> shader = std::make_shared <gp::Shader> ();
		shader->add_source(gp::Shader_type::SHADER_TYPE_VERTEX, "vertex.glsl");
		shader->add_source(gp::Shader_type::SHADER_TYPE_FRAGMENT, "fragment.glsl");
		shader->assemble();
		
		gp::Rect rect(shader);
		
		gp::launch_main_loop([&] () -> void {
			rect.render();
		});
	}
	
	gp::terminate();
}
