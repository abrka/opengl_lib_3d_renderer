#pragma once

#include <vector>
#pragma once

#include <vector>
#include <entt/fwd.hpp>
#include "parent_component.h"

namespace Engine {
	struct ChildrenComponent {
		std::vector<entt::entity> children{};
	};
	void add_child(entt::registry& entt_registry, entt::entity parent_entity, entt::entity child_entity);
	void destroy_entity(entt::registry& entt_registry, entt::entity entity);

}