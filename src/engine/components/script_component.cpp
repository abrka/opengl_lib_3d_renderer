#include "script_component.h"

namespace Engine {
	tl::expected<ScriptComponent, sol::error> build_script_component(sol::state& sol_state, std::filesystem::path script_filepath) {
		auto result = sol_state.script_file(script_filepath.string(), [](lua_State*, sol::protected_function_result pfr) {
			return pfr;
			});
		if (!result.valid()) {
			sol::error sol_error = result;
			return tl::make_unexpected(sol_error);
		}
		sol::table sol_module = result;
		if (!sol_module.valid()) {
			return tl::make_unexpected(sol::error{ "[ENGINE] No module in lua script" });
		}
		return ScriptComponent{ sol_module, script_filepath };
	}
}