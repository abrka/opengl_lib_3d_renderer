#pragma once

#include <filesystem>
#include <tl/expected.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
	struct ScriptComponent {
		std::filesystem::path filepath{};
		tl::expected<sol::table,sol::error> sol_result{};
	};
	
	ScriptComponent build_script_component(sol::state& sol_state, std::filesystem::path script_filepath) {
		auto result = sol_state.script_file(script_filepath.string(), [](lua_State*, sol::protected_function_result pfr) {
			return pfr;
			});
		if (!result.valid()) {
			sol::error sol_error = result;
			return { script_filepath, tl::make_unexpected(sol_error) };
		}
		std::optional<sol::table> sol_module = result;
		if (!sol_module.has_value()) {
			return { script_filepath, tl::make_unexpected(sol::error{ "[ENGINE] No module in lua script" }) };
		}
		return { script_filepath, sol_module.value() };
	}

	void add_script_component(sol::state& sol_state, entt::registry& entt_registry, entt::entity entity, std::filesystem::path script_filepath) {
		ScriptComponent script_component = build_script_component(sol_state, script_filepath);
		sol::table& lua_module = *script_component.sol_result;
		lua_module["entity_id"] = entt::to_integral(entity);
		entt_registry.emplace<ScriptComponent>(entity, script_component);
	}
}