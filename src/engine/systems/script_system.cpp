#include "script_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include "engine/components/script_component.h"

namespace Engine {
	static tl::expected<void, sol::error> call_sol_module_init_function(sol::table& sol_module)
	{
		sol::function sol_init_fn = sol_module["init"];
		if (!sol_init_fn.valid()) {
			return tl::make_unexpected(sol::error{ "lua module does not have an init function" });
		}
		auto sol_ret = sol_init_fn.call(sol_module);
		if (!sol_ret.valid()) {
			sol::error sol_err = sol_ret;
			return tl::make_unexpected(sol_err);
		}
		return {};
	}
	void script_system_init(entt::registry& entt_registry)
	{
		auto entt_view_scripts = entt_registry.view<Engine::ScriptComponent>();
		for (auto [entity, script] : entt_view_scripts.each()) {
			auto result_init = call_sol_module_init_function(script.sol_module);
			if (!result_init) {
				std::cout << "[ERROR][LUA] init function : " << result_init.error().what() << "\n";
			}
		}
	}
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