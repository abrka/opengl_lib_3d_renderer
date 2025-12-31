#include "script_component.h"
#include <entt/entt.hpp>
#include <tl/expected.hpp>

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
	static tl::expected<void,sol::error> call_sol_module_init_function(sol::table& sol_module)
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
	tl::expected<ScriptComponent, sol::error> ScriptComponent::build(entt::registry& entt_registry, sol::state& sol_state, std::filesystem::path script_filepath, entt::entity entity) {
		auto result = sol_state.script_file(script_filepath.string(), [](lua_State*, sol::protected_function_result pfr) {
			return pfr;
			});
		if (!result.valid()) {
			sol::error sol_error = result;
			return tl::make_unexpected(sol_error);
		}
		sol::table sol_module = result;
		if (!sol_module.valid()) {
			return tl::make_unexpected(sol::error{ "lua script does not return a module table" });
		}
		set_sol_module_variables(sol_module, entt_registry, entity);
		auto result_init = call_sol_module_init_function(sol_module);
		if (!result_init) {
			return tl::make_unexpected(result_init.error());
		}
		return ScriptComponent{ std::move(sol_module) };
	}
}