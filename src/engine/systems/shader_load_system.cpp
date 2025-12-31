#include "shader_load_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include "engine/components/shader_component.h"
#include "engine/components/shader_info_component.h"

namespace Engine {
	void shader_load_system(entt::registry& entt_registry, entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader>& cache)
	{
		auto entt_view_shader_infos = entt_registry.view<Engine::ShaderInfoComponent>(entt::exclude<Engine::ShaderComponent>);
		for (auto [entity, shader_info] : entt_view_shader_infos.each()) {
			if (shader_info.fragment_filepath.empty()) {
				continue;
			}
			if (shader_info.vertex_filepath.empty()) {
				continue;
			}
			entt::hashed_string hash{ (shader_info.vertex_filepath.string() + shader_info.fragment_filepath.string()).c_str() };
			auto result = cache.load(hash, shader_info.fragment_filepath, shader_info.vertex_filepath).first->second;
			if (!result) {
				std::cerr << "[ERROR][ENGINE][SHADER LOAD SYSTEM]: " << Engine::ShaderLoader::get_last_error().err_msg << "\n";
				continue;
			}
			entt_registry.emplace<Engine::ShaderComponent>(entity, Engine::ShaderComponent{ result });
		}
	}
}
