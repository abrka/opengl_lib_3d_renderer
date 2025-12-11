#pragma once

#include <memory>
#include <entt/entt.hpp>
#include <GL3D/shader_program.h>

namespace Engine {
	using ShaderComponent = entt::resource<GL3D::ShaderProgram>;
}