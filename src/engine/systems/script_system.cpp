#include "script_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include "engine/components/script_component.h"

namespace Engine {
	static void set_sol_module_variables(sol::table& sol_module, entt::registry& entt_registry, entt::entity entity)
	{
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
	}
	static void call_sol_module_init_function(sol::table& sol_module)
	{
		sol::function sol_init_fn = sol_module["init"];
		if (!sol_init_fn.valid()) {
			return;
		}
		auto sol_ret = sol_init_fn.call(sol_module);
		if (!sol_ret.valid()) {
			sol::error sol_err = sol_ret;
			std::cout << "[ERROR][LUA] init function : " << sol_err.what() << "\n";
		}
	}
	static void sol_module_init(entt::registry& entt_registry, sol::state& sol_state, entt::entity entity, Engine::ScriptComponent& script_component)
	{
		// first setup necessary lua variables
		set_sol_module_variables(script_component.sol_module, entt_registry, entity);
		// then call init function
		call_sol_module_init_function(script_component.sol_module);
	}
	void sol_module_construct(sol::state& sol_state, Engine::ScriptComponent& script_component)
	{
		auto script_result = Engine::ScriptComponent::build(sol_state, script_component.filepath);
		if (!script_result) {
			std::cout << "[ERROR][ENGINE][SCRIPT SYSTEM INIT]: couldnt load script: " << script_result.error().what() << "\n";
			return;
		}
		script_component = script_result.value();
	}
	void script_system_init(entt::registry& entt_registry, sol::state& sol_state) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			if (!script_component.sol_module.valid()) {
				sol_module_construct(sol_state, script_component);
			}
			sol_module_init(entt_registry, sol_state, entity, script_component);
		}
	}

	void script_system_tick(entt::registry& entt_registry, sol::state& sol_state) {
		auto entt_view_scripts = entt_registry.view<ScriptComponent>();
		for (auto [entity, script_component] : entt_view_scripts.each()) {
			auto& sol_module = script_component.sol_module;

			if (!script_component.sol_module.valid()) {
				sol_module_construct(sol_state, script_component);
				sol_module_init(entt_registry, sol_state, entity, script_component);
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