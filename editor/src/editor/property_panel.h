#pragma once

#include <optional>
#include <entt/fwd.hpp>

namespace Editor {
	class PropertyPanel {
	public:
		PropertyPanel(entt::registry& entt_registry);
		void render(std::optional<entt::entity> selected_entity);
	private:
		entt::registry* entt_registry{};
		void render_components(entt::entity& entity);
		void render_add_component_popup(entt::entity& entity);
	};
}