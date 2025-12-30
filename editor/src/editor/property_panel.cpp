#include "property_panel.h"

#include <entt/entt.hpp>
#include <imgui.h>

namespace Editor {
	PropertyPanel::PropertyPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {};

	void PropertyPanel::render(std::optional<entt::entity> selected_entity) {
		ImGui::Begin("Components");
		if (selected_entity.has_value()) {
			if (ImGui::Button("[+] Add Component")) {
				ImGui::OpenPopup("Add Component Popup");
			}
			if (ImGui::BeginPopup("Add Component Popup")) {
				render_add_component_popup(selected_entity.value());
				ImGui::EndPopup();
			}
			render_components(selected_entity.value());
		}
		ImGui::End();
	}
	void PropertyPanel::render_components(entt::entity& entity)
	{
		using namespace entt::literals;
		for (auto&& [id, type] : entt::resolve()) {
			entt::meta_func get_comp_fn = type.func("get_component"_hs);
			assert(get_comp_fn);
			auto returned_component = get_comp_fn.invoke({}, entt_registry, entity);
			assert(returned_component);

			auto component_type = returned_component.type().remove_pointer();
			assert(component_type);
			const char* component_name = component_type.name();

			entt::meta_func render_comp_fn = type.func("render_imgui_for_component"_hs);
			assert(render_comp_fn);
			auto ret = render_comp_fn.invoke({}, component_name, returned_component);
			assert(ret);
		}
	}
	void PropertyPanel::render_add_component_popup(entt::entity& entity)
	{
		using namespace entt::literals;
		for (auto&& [id, type] : entt::resolve()) {
			entt::meta_func get_comp_fn = type.func("get_component"_hs);
			assert(get_comp_fn);
			auto returned_component = get_comp_fn.invoke({}, entt_registry, entity);
			assert(returned_component);

			auto component_type = returned_component.type().remove_pointer();
			assert(component_type);
			const char* component_name = component_type.name();

			if (ImGui::Button(component_name)) {
				entt::meta_func add_comp_fn = type.func("add_component"_hs);
				assert(add_comp_fn);
				auto ret = add_comp_fn.invoke({}, entt_registry, entity);
				assert(ret);
			}
		}
	}
}