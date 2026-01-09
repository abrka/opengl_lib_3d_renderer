#pragma once 

#include <iostream>
#include <memory>

#include <entt/fwd.hpp>

#include <GLRenderer/renderer_base.h>
#include <GLExternalRAII/glfw_window_raii.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GL3D {
	class Mesh;
	class Texture;
	class ShaderProgram;
	class Framebuffer;
	class Renderbuffer;
}

namespace Renderer {
	class Renderer3D : public GLRenderer::RendererBase
	{
	public:
		std::function<void(Renderer3D&)> custom_imgui_render_function{};
		std::function<void(Renderer3D&)> custom_render_function{};
		Renderer3D(GLExternalRAII::Window& window, entt::registry& entt_registry);
		void on_window_resize(int width, int height);
		void render_user() override;
		virtual ~Renderer3D();
	private:
		entt::registry* entt_registry{};
		std::unique_ptr<GL3D::Mesh> screen_quad_mesh{};
		std::unique_ptr<GL3D::ShaderProgram> screen_shader{};
		std::unique_ptr<GL3D::Framebuffer> framebuffer{};
		std::unique_ptr<GL3D::Texture> framebuffer_texture{};
		std::unique_ptr<GL3D::Renderbuffer> framebuffer_renderbuffer{};
		void create_screen_framebuffer(int width, int height);

	};
}