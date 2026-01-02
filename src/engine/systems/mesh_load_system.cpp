#include "mesh_load_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include "engine/components/mesh_component.h"
#include "engine/components/mesh_info_component.h"

namespace Engine {
	void mesh_load_system(entt::registry& entt_registry, entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader>& cache)
	{
		auto entt_view_mesh_infos = entt_registry.view<Engine::MeshInfoComponent>();
		for (auto [entity, mesh_info] : entt_view_mesh_infos.each()) {
			if (mesh_info.filepath.empty()) {
				continue;
			}
			if (!mesh_info.requires_reload) {
				continue;
			}
			entt::hashed_string hash{mesh_info.filepath.string().c_str()};
			auto result = cache.load(hash, mesh_info.filepath).first->second;
			if (!result) {
				std::cerr << "[ERROR][ENGINE][MESH LOAD SYSTEM]: " << Engine::MeshLoader::get_last_error() << "\n";
				continue;
			}
			entt_registry.emplace_or_replace<Engine::MeshComponent>(entity, Engine::MeshComponent{ result });
			mesh_info.requires_reload = false;
		}
	}
}
