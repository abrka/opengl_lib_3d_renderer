#pragma once

#include <optional>
#include <entt/entt.hpp>
#include <imgui.h>

namespace Editor {
	class PropertyPanel {
	public:
		PropertyPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {};

		void render(std::optional<entt::entity> selected_entity) {
			ImGui::Begin("Components");
			if (selected_entity.has_value()) {
				render_components(selected_entity.value());

				if (ImGui::Button("[+] Add Component")) {
					ImGui::OpenPopup("Add Component Popup");
					is_add_component_popup_open = true;
				}
				if (ImGui::BeginPopupModal("Add Component Popup", &is_add_component_popup_open)) {
					render_add_component_popup(selected_entity.value());
					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}
	private:
		entt::registry* entt_registry{};
		bool is_add_component_popup_open = false;

		void render_components(entt::entity& entity)
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
		void render_add_component_popup(entt::entity& entity)
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
					is_add_component_popup_open = false;
				}
			}
		}
	};
}