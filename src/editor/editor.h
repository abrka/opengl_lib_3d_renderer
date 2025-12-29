#pragma once

#include <entt/fwd.hpp>
#include <sol/forward.hpp>

#include "engine/serialize/entt_registry_serializer.h"
#include "hierarchical_panel.h"
#include "property_panel.h"

namespace Renderer {
	class Renderer3D;
}
namespace Editor {
	class Editor3D {
	public:
		Editor3D(entt::registry& entt_registry, sol::state& sol_state, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer);
		void render();
		void render_top_bar();
	private:
		entt::registry* entt_registry;
		Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer;
		sol::state* sol_state;
		HierarchicalPanel hierarchical_panel;
		PropertyPanel component_panel;
		bool is_scripts_running{ false };
		bool is_first_time_running_scripts{ true };

		void render_save_load_panel();
		void render_imguizmo();
	};
}