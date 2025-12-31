#pragma once

#include <filesystem>
#include <memory>
#include <entt/entt.hpp>

namespace GL3D {
	class ShaderProgram;
}
namespace Engine {
	struct ShaderComponent {
		entt::resource<GL3D::ShaderProgram> shader{};
	};
}