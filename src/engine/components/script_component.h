#pragma once

#include <filesystem>
#include <tl/expected.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
	struct ScriptComponent {
		std::filesystem::path filepath{};
		tl::expected<sol::table,sol::error> sol_result{};
		ScriptComponent(sol::state& sol_state, std::filesystem::path filepath) : filepath(filepath) {
			auto result = sol_state.script_file(filepath.string(), [](lua_State*, sol::protected_function_result pfr) {
				return pfr;
			});
			if (!result.valid()) {
				sol::error sol_error = result;
				sol_result = tl::make_unexpected(sol_error);
				return;
			}
			std::optional<sol::table> sol_module = result;
			if (!sol_module.has_value()) {
				sol_result = tl::make_unexpected(sol::error{ "[ENGINE] No module in lua script" });
				return;
			}
			sol_result = sol_module.value();
		}
	};
}