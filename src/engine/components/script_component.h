#pragma once

#include <filesystem>
#include <tl/expected.hpp>
#include <entt/fwd.hpp>
#include <entt/entity/fwd.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
	struct ScriptComponent {
		sol::table sol_module{};
		static tl::expected<ScriptComponent, sol::error> build(entt::registry& entt_registry, sol::state& sol_state, std::filesystem::path script_filepath, entt::entity entity);
	};
}