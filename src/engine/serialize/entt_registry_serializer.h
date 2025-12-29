#pragma once

#include <functional>
#include <istream>
#include <ostream>

#include <entt/entt.hpp>
#include <cereal/archives/xml.hpp>

#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"
#include "engine/systems/script_system.h"
#include "cereal_serialize_custom_types.h"

namespace Engine {
	template<typename InputArchive, typename OutputArchive>
	struct EnttRegistrySerializer {
		using snapshot_get_func = std::function<void(OutputArchive&, entt::snapshot&)>;
		snapshot_get_func snapshot_get_fn{};
		using snapshot_loader_get_func = std::function<void(InputArchive&, entt::snapshot_loader&)>;
		snapshot_loader_get_func snapshot_loader_get_fn{};
		sol::state* sol_state{};

		EnttRegistrySerializer(snapshot_get_func f1, snapshot_loader_get_func f2, sol::state& sol_state) : snapshot_get_fn(f1), snapshot_loader_get_fn(f2), sol_state(&sol_state) {};

		void save(entt::registry& entt_registry, OutputArchive& archive) {
			entt::snapshot snapshot{ entt_registry };
			snapshot.get<entt::entity>(archive);
			snapshot_get_fn(archive, snapshot);
		}
		void save(entt::registry& entt_registry, std::ostream& os) {
			OutputArchive archive{ os };
			save(entt_registry, archive);
		}

		void load(entt::registry& entt_registry, InputArchive& archive) {
			entt_registry.clear();
			entt::snapshot_loader snapshot_loader{ entt_registry };
			snapshot_loader.get<entt::entity>(archive);
			snapshot_loader_get_fn(archive, snapshot_loader);

			entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> entt_shader_cache{};
			auto entt_view_shaders = entt_registry.view<Engine::ShaderComponent>();
			for (auto [entity, shader_component] : entt_view_shaders.each()) {
				auto hash = entt::hashed_string((shader_component.fragment_filepath.string() + shader_component.vertex_filepath.string()).c_str());
				auto result = entt_shader_cache.load(hash, shader_component.fragment_filepath, shader_component.vertex_filepath);
				assert(result.first->second);
				shader_component.shader = result.first->second;
			}

			entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader> entt_mesh_cache{};
			auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
			for (auto [entity, mesh_component] : entt_view_meshes.each()) {
				auto hash = entt::hashed_string(mesh_component.filepath.string().c_str());
				auto result = entt_mesh_cache.load(hash, mesh_component.filepath);
				assert(result.first->second);
				mesh_component.scene = result.first->second;
			}

			// No need to build script components since they get lazy loaded in Engine::script_system_init
		}
		void load(entt::registry& entt_registry, std::istream& is) {
			InputArchive archive{ is };
			load(entt_registry, archive);
		}
	};
}