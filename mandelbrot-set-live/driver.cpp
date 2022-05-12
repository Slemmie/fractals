#include "gp.h"
#include "fps.h"

#include <memory>

int main() {
	gp::init();
	
	{
		std::shared_ptr <gp::Shader> shader = std::make_shared <gp::Shader> ();
		shader->add_source(gp::Shader_type::SHADER_TYPE_VERTEX, "vertex.glsl");
		shader->add_source(gp::Shader_type::SHADER_TYPE_FRAGMENT, "fragment.glsl");
		shader->assemble();
		
		float zoom = 6.290223e+3;
		float zoom_x = -1.186592e+0;
		float zoom_y = -1.901211e-1;
		
		shader->bind();
		shader->set_uniform_1i("u_max_iterations", 10'000);
		shader->set_uniform_1f("u_zoom", zoom);
		shader->set_uniform_1f("u_zoom_x", zoom_x);
		shader->set_uniform_1f("u_zoom_y", zoom_y);
		shader->set_uniform_1f("u_r_begin", -2.0f / zoom + zoom_x);
		shader->set_uniform_1f("u_r_end", 2.0f / zoom + zoom_x);
		shader->set_uniform_1f("u_i_begin", -2.0f / zoom - zoom_y);
		shader->set_uniform_1f("u_i_end", 2.0f / zoom - zoom_y);
		shader->unbind();
		
		gp::Rect rect(shader);
		
		Fps fps;
		Fps::time_stamp since_fps_update = 0;
		gp::launch_main_loop([&] () -> void {
			fps.push_frame();
			if (Fps::since_epoch() - since_fps_update >= static_cast <Fps::time_stamp> (1'000'000)) {
				gp::set_window_title(std::to_string(fps.fps()));
				since_fps_update = Fps::since_epoch();
			}
			rect.render();
		});
	}
	
	gp::terminate();
}
