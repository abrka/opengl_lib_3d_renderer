#pragma once

#include <entt/fwd.hpp>
#include <sol/forward.hpp>

#include "engine/serialize/entt_registry_serializer.h"
#include "hierarchical_panel.h"
#include "property_panel.h"
#include "jolt_debug_renderer.h"

namespace Renderer {
	class Renderer3D;
}
namespace Editor {
	class Editor3D {
	public:
		Editor3D(entt::registry& entt_registry, sol::state& sol_state, Physics::World& physics_world, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer);
		void render();
	private:
		entt::registry* entt_registry{};
		sol::state* sol_state{};
		Physics::World* physics_world{};
		Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer;

		HierarchicalPanel hierarchical_panel;
		PropertyPanel component_panel;
		JoltDebugRenderer jolt_debug_renderer;

		std::optional<std::filesystem::path> last_saved_scene{};

		enum class GameState {
			RUNNING,
			PAUSED,
			STOPPED
		};
		GameState game_state{ GameState::STOPPED };

		void reload_scene_from_last_saved_file();
		void save_scene_to_last_saved_file();
		void save_scene_popup();

		void render_top_bar();
		void render_stop_scene_button();
		void render_save_button();
		void render_load_button();
		void render_save_load_panel();
		void render_imguizmo();
		void render_jolt_shape_of_entity(entt::entity entity);
		void render_jolt_debug();
	};
}