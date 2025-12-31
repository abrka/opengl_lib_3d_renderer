#pragma once

#include <entt/fwd.hpp>
#include "renderer/asset_builder.h"
#include "engine/loaders/mesh_loader.h"

namespace Engine {
	void mesh_load_system(entt::registry& entt_registry, entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader>& cache);
}