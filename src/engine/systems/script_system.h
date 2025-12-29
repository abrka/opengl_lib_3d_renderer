#pragma once

#include <entt/fwd.hpp>
#include <sol/forward.hpp>

namespace Engine {
	void script_system_init(entt::registry& entt_registry, sol::state& sol_state);
	void script_system_reload(entt::registry& entt_registry, sol::state& sol_state);
	void script_system_tick(entt::registry& entt_registry, sol::state& sol_state);
}