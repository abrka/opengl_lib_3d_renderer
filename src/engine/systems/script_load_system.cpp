#include "script_load_system.h"

#include <iostream>
#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include "engine/components/script_component.h"
#include "engine/components/script_info_component.h"

namespace Engine {
	void script_load_system(entt::registry& entt_registry, sol::state& sol_state)
	{
		auto entt_view_script_infos = entt_registry.view<Engine::ScriptInfoComponent>(entt::exclude<Engine::ScriptComponent>);
		for (auto [entity, script_info] : entt_view_script_infos.each()) {
			if (script_info.filepath.empty()) {
				continue;
			}
			auto script_component_result = Engine::ScriptComponent::build(entt_registry, sol_state, script_info.filepath, entity);
			if (!script_component_result) {
				std::cerr << "[ERROR][ENGINE][SCRIPT LOAD SYSTEM]: " << script_component_result.error().what() << "\n";
				continue;
			}
			entt_registry.emplace<Engine::ScriptComponent>(entity, *script_component_result);
		}
	}
}
