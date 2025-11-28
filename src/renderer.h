#pragma once 

#include "GL3D/mesh.h"
#include "GL3D/shader.h"
#include "GL3D/texture.h"
#include "GL3D/framebuffer.h"
#include "GL3D/renderbuffer.h"

#include "GLRenderer/renderer_base.h"

#include "GLExternalRAII/glfw_window_raii.h"

#include "GLExternalUtils/glad_util.h"
#include "GLExternalUtils/imgui_util.h"
#include "GLExternalUtils/opengl_util.h"

#include "shader_builder.h"
#include "mesh_builder.h"
#include "texture_builder.h"
#include "camera.h"
#include "scene_renderer.h"
#include "stb_image_raii.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)



class Renderer : public GLRenderer::RendererBase
{
public:
	Camera cam{};
	std::vector<MeshBuilder::Scene> scenes{};

private:
	std::unique_ptr<GL3D::ShaderProgram> pbr_shader{};

	std::unique_ptr<GL3D::Mesh> screen_quad_mesh{};
	std::unique_ptr<GL3D::ShaderProgram> screen_shader{};

	std::unique_ptr<GL3D::Framebuffer> framebuffer{};
	std::unique_ptr<GL3D::Texture> framebuffer_texture{};
	std::unique_ptr<GL3D::Renderbuffer> framebuffer_renderbuffer{};


public:
	Renderer(std::shared_ptr<GLExternalRAII::Window> window) : RendererBase(window) {
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

		const std::string asset_dir = std::string(TOSTRING(ASSET_DIR)) + "/";

		auto pbr_shader_res = GLRenderer::ShaderBuilder::build(asset_dir + "shaders/pbr_frag.glsl", asset_dir + "shaders/pbr_vertex.glsl");
		if (!pbr_shader_res.has_value()) {
			std::cout << pbr_shader_res.error().err_msg << "\n";
			assert(false);
		}
		pbr_shader = std::move(pbr_shader_res.value());

		auto screen_shader_res = GLRenderer::ShaderBuilder::build(asset_dir + "shaders/screen_frag.glsl", asset_dir + "shaders/screen_vertex.glsl");
		if (!screen_shader_res.has_value()) {
			std::cout << screen_shader_res.error().err_msg << "\n";
			assert(false);
		}
		screen_shader = std::move(screen_shader_res.value());

		create_screen_framebuffer();

	}

	void render_user() override {
		auto [screen_width, screen_height] = window->get_width_and_height();
		cam.aspect_ratio = screen_width / screen_height;

		framebuffer->bind();

		glClearColor(29.0f / 255.0f, 30.0f / 255.0f, 39.0f / 255.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND); // enable blending function
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (const auto& scene : scenes) {
			draw_scene(cam, scene, *pbr_shader);
		}
		
		framebuffer->unbind();

		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		screen_shader->set_texture("screen_texture", *framebuffer_texture, 0);
		screen_quad_mesh->draw(*screen_shader);
	}
	void on_window_resize(int width, int height) {
		glViewport(0, 0, width, height);
		create_screen_framebuffer();
	}
private:
	void create_screen_framebuffer() {
		framebuffer = std::make_unique<GL3D::Framebuffer>();
		auto [window_width, window_height] = window->get_width_and_height();
		framebuffer_texture = std::make_unique<GL3D::Texture>(window_width, window_height, std::span<unsigned char>{}, GL3D::TextureSpec{ .generate_mipmap = false });
		framebuffer->attach_texture(*framebuffer_texture);
		framebuffer_renderbuffer = std::make_unique<GL3D::Renderbuffer>(GL_DEPTH24_STENCIL8, window_width, window_height);
		framebuffer->attach_renderbuffer(*framebuffer_renderbuffer);
		assert(framebuffer->get_status());
	}
};
