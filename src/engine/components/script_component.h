#pragma once

#include <filesystem>
#include <tl/expected.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
	struct ScriptComponent {
		sol::table sol_module{};
		std::filesystem::path filepath{};
	};
	
	tl::expected<ScriptComponent, sol::error> build_script_component(sol::state& sol_state, std::filesystem::path script_filepath);
}