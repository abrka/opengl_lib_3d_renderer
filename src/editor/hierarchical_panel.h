#pragma once

#include <optional>

#include <entt/fwd.hpp>
#include <imgui.h>


namespace Editor {
	class HierarchicalPanel
	{
	public:
		std::optional<entt::entity> selected_entity{ std::nullopt };

		HierarchicalPanel(entt::registry& entt_registry);
		void render();
	private:
		entt::registry* entt_registry{};
		size_t added_entity_count{};
		void render_add_button();
		void render_remove_button();
		void render_entities(entt::entity entity);

	};
}
