#pragma once

#include <optional>
#include <entt/entt.hpp>
#include "engine/components/script_component.h"

namespace Engine {
	void script_system_run(entt::registry& entt_registry) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			if (!script_component.sol_result.has_value()) {
				continue;
			}
			auto& sol_module = script_component.sol_result.value();
			std::optional<sol::function> sol_tick_fn = sol_module["tick"];
			if (!sol_tick_fn.has_value()) {
				continue;
			}
			auto sol_ret = sol_tick_fn->call(sol_module);
			if (!sol_ret.valid()) {
				sol::error sol_err = sol_ret;
				std::cout << "[ERROR] Lua Script Error: " << sol_err.what() << "\n";
			}
		}
	}
}