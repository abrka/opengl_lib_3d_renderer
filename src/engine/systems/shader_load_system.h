#pragma once

#include <entt/fwd.hpp>
#include <GL3D/shader_program.h>
#include "engine/loaders/shader_loader.h"

namespace Engine {
	void shader_load_system(entt::registry& entt_registry, entt::resource_cache<GL3D::ShaderProgram,Engine::ShaderLoader>& cache);
}