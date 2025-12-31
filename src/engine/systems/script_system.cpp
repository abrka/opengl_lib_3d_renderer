#include "script_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include "engine/components/script_component.h"

namespace Engine {
	void script_system_tick(entt::registry& entt_registry, sol::state& sol_state) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			auto& sol_module = script_component.sol_module;
			assert(sol_module.valid());
			sol::function sol_tick_fn = sol_module["tick"];
			if (!sol_tick_fn.valid()) {
				continue;
			}
			auto sol_ret = sol_tick_fn.call(sol_module);
			if (!sol_ret.valid()) {
				sol::error sol_err = sol_ret;
				std::cout << "[ERROR][LUA] tick function : " << sol_err.what() << "\n";
			}
		}
	}
}