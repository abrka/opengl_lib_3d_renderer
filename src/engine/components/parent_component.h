#pragma once

#include <optional>
#include <entt/entt.hpp>

namespace Engine {
	struct ParentComponent {
		entt::entity entity{ entt::null };
		ParentComponent() = default;
		ParentComponent(entt::entity entity) : entity(entity) {};
	};
}