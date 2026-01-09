#include "renderer.h"

#include <functional>
#include <entt/entt.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <GL3D/mesh.h>
#include <GL3D/shader_program.h>
#include <GL3D/texture.h>
#include <GL3D/framebuffer.h>
#include <GL3D/renderbuffer.h>

#include "shader_builder.h"
#include "camera.h"
#include "renderer_detail.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)



namespace Renderer {
	Renderer3D::Renderer3D(GLExternalRAII::Window& window, entt::registry& entt_registry) : RendererBase(window), entt_registry(&entt_registry) {
		struct Vertex2 {
			glm::vec3 position{};
			glm::vec2 texCoord{};
		};
		float screen_quad_vertices[] = {
			-1.0f, 1.0f,0.0f, 0.0f,1.0f,
			 1.0f, 1.0f,0.0f, 1.0f,1.0f,
			 1.0f,-1.0f,0.0f, 1.0f,0.0f,
			-1.0f,-1.0f,0.0f, 0.0f,0.0f,
		};
		unsigned int quad_indices[] = {
			0,1,3,1,2,3
		};
		int num_floats_per_attr[] = { 3,2 };
		screen_quad_mesh = std::make_unique<GL3D::Mesh>(std::span<float>(screen_quad_vertices), std::span<int>(num_floats_per_attr), std::span<unsigned int>(quad_indices));

		const std::string asset_dir = std::string(TOSTRING(ENGINE_ASSET_DIR)) + "/";

		auto screen_shader_res = ShaderBuilder::build(asset_dir + "shaders/screen_frag.glsl", asset_dir + "shaders/screen_vertex.glsl");
		if (!screen_shader_res.has_value()) {
			std::cout << screen_shader_res.error().err_msg << "\n";
			assert(false);
		}
		screen_shader = std::move(screen_shader_res.value());

		auto [width, height] = window.get_width_and_height();
		create_screen_framebuffer(width, height);

	}
	void Renderer3D::render_user() {
		ImGuizmo::BeginFrame();

		framebuffer->bind();

		if (custom_imgui_render_function) {
			custom_imgui_render_function(*this);
		}

		glClearColor(29.0f / 255.0f, 30.0f / 255.0f, 39.0f / 255.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND); // enable blending function
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto [screen_width, screen_height] = window->get_width_and_height();
		auto entt_view_camera = entt_registry->view<Engine::CameraComponent>();
		for (auto [entity, camera_component] : entt_view_camera.each()) {
			camera_component.camera.aspect_ratio = (float)screen_width / (float)screen_height;
		}

		size_t num_point_lights = 10;
		detail::set_light_uniforms_system(*entt_registry, num_point_lights);
		detail::set_mvp_uniforms_system(*entt_registry);
		detail::set_camera_uniform_system(*entt_registry);
		detail::render_mesh_system(*entt_registry);
		if (custom_render_function) {
			custom_render_function(*this);
		}

		framebuffer->unbind();

		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		screen_shader->set_texture("screen_texture", *framebuffer_texture, 0);
		screen_quad_mesh->draw(*screen_shader);
	}
	void Renderer3D::on_window_resize(int width, int height) {
		create_screen_framebuffer(width, height);
		glViewport(0, 0, width, height);
	}
	void Renderer3D::create_screen_framebuffer(int width, int height) {
		framebuffer = std::make_unique<GL3D::Framebuffer>();
		framebuffer_texture = std::make_unique<GL3D::Texture>(width, height, std::span<unsigned char>{}, GL3D::TextureSpec{ .generate_mipmap = false });
		framebuffer->attach_texture(*framebuffer_texture);
		framebuffer_renderbuffer = std::make_unique<GL3D::Renderbuffer>(GL_DEPTH24_STENCIL8, width, height);
		framebuffer->attach_renderbuffer(*framebuffer_renderbuffer);
		assert(framebuffer->get_status());
	}
	Renderer3D::~Renderer3D() = default;
}