#pragma once

#include <memory>
#include <GL3D/shader_program.h>

namespace Engine {
	using ShaderComponent = std::shared_ptr<GL3D::ShaderProgram>;
}