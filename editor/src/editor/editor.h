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

		bool is_scripts_running{ false };
		bool is_physics_running{ false };

		void render_top_bar();
		void render_save_button();
		void render_load_button();
		void render_save_load_panel();
		void render_imguizmo();
		void render_jolt_shape_of_selected_entity();
		void render_jolt_debug();
	};
}