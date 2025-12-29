#include "script_system.h"

#include <entt/entt.hpp>
#include "engine/components/script_component.h"

namespace Engine {
	void script_system_init(entt::registry& entt_registry) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			auto& sol_module = script_component.sol_module;
			if (!sol_module.valid()) {
				continue;
			}
			// first setup necessary lua variables
			sol_module["entity_id"] = entt::to_integral(entity);
			sol_module["components"] = sol::new_table{};
			for (auto&& [id, type] : entt::resolve()) {
				using namespace entt::literals;
				entt::meta_func get_comp_fn = type.func("get_component"_hs);
				assert(get_comp_fn);
				auto returned_component = get_comp_fn.invoke({}, &entt_registry, entity);
				assert(returned_component);
				entt::meta_func add_component_to_sol_fn = type.func("sol_add_component"_hs);
				assert(add_component_to_sol_fn);
				auto ret = add_component_to_sol_fn.invoke({}, &sol_module, returned_component, type.name());
				assert(ret);
			}

			// then call init function
			sol::function sol_init_fn = sol_module["init"];
			if (!sol_init_fn.valid()) {
				continue;
			}
			auto sol_ret = sol_init_fn.call(sol_module);
			if (!sol_ret.valid()) {
				sol::error sol_err = sol_ret;
				std::cout << "[ERROR][LUA] init function : " << sol_err.what() << "\n";
			}
		}
	}

	void script_system_tick(entt::registry& entt_registry) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			auto& sol_module = script_component.sol_module;
			if (!sol_module.valid()) {
				continue;
			}
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