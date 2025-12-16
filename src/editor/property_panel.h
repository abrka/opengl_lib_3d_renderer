#pragma once

#include <entt/entt.hpp>
#include <imgui.h>

namespace Editor {
	namespace PropertyPanel {
		void render(entt::registry& entt_registry, entt::entity entity) {
			ImGui::Begin("Properties");
			using namespace entt::literals;

			for (auto&& [id, type] : entt::resolve()) {
				entt::meta_func get_comp_fn = type.func("get_component"_hs);
				assert(get_comp_fn);
				auto returned_component = get_comp_fn.invoke({}, &entt_registry, entity);
				assert(returned_component);
				entt::meta_func render_comp_fn = type.func("render_imgui_for_component"_hs);
				assert(render_comp_fn);
				auto component_type = returned_component.type().remove_pointer();
				assert(component_type);
				const char* component_name = component_type.name();
				auto ret = render_comp_fn.invoke({},component_name, returned_component);
				assert(ret);
			}
			ImGui::End();
		}
	}
}