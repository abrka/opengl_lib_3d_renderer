#pragma once

#include <optional>
#include <entt/entt.hpp>

namespace Engine {
	using ParentComponent = std::optional<entt::entity>;
}