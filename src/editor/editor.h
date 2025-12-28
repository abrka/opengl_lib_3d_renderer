#pragma once

#include <entt/fwd.hpp>

#include "engine/serialize/entt_registry_serializer.h"
#include "hierarchical_panel.h"
#include "property_panel.h"

namespace Renderer {
	class Renderer3D;
}
namespace ImGuizmo {
	enum OPERATION;
	enum MODE;
}
namespace Editor {
	class Editor3D {
	public:
		Editor3D(entt::registry& entt_registry, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer);
		void render();

	private:
		entt::registry* entt_registry;
		Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer{};
		HierarchicalPanel hierarchical_panel;
		PropertyPanel component_panel;
		ImGuizmo::OPERATION imguizmo_operation{};
		ImGuizmo::MODE imguizmo_mode{};

		void render_save_load_panel();
	};
}