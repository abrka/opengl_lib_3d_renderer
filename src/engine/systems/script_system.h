#pragma once

#include <entt/fwd.hpp>

namespace Engine {
	void script_system_init(entt::registry& entt_registry);
	void script_system_tick(entt::registry& entt_registry);
}