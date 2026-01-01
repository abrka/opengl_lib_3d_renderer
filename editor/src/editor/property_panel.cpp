#include "property_panel.h"

#include <entt/entt.hpp>
#include <imgui.h>
#include "reflect/reflect.h"

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

		size_t i = 0;
		for (auto&& [id, type] : entt::resolve()) {
			entt::meta_func get_comp_fn = type.func(Reflect::get_component_func_name);
			assert(get_comp_fn);
			auto returned_component = get_comp_fn.invoke({}, entt_registry, entity);
			assert(returned_component);

			entt::meta_func render_comp_fn = type.func(Reflect::render_imgui_for_component_func_name);
			assert(render_comp_fn);

			const char* component_name = type.name();
			auto ret = render_comp_fn.invoke({}, component_name, returned_component);
			assert(ret);

			entt::meta_func is_component_null_fn = type.func(Reflect::is_null_func_name);
			assert(is_component_null_fn);
			auto is_null = is_component_null_fn.invoke({}, returned_component);
			assert(is_null);
			if (!is_null.cast<bool>()) {
				ImGui::PushID(i);
				ImGui::Spacing();
				if (ImGui::Button("[-] Remove Component")) {
					entt::meta_func remove_component_fn = type.func(Reflect::remove_component_func_name);
					assert(remove_component_fn);
					auto ret = remove_component_fn.invoke({}, entt_registry, entity);
					assert(ret);
				}
				ImGui::PopID();
			}
			i++;
		}
	}
	void PropertyPanel::render_add_component_popup(entt::entity& entity)
	{
		using namespace entt::literals;
		for (auto&& [id, type] : entt::resolve()) {
			entt::meta_func get_comp_fn = type.func(Reflect::get_component_func_name);
			assert(get_comp_fn);
			auto returned_component = get_comp_fn.invoke({}, entt_registry, entity);
			assert(returned_component);

			auto component_type = returned_component.type().remove_pointer();
			assert(component_type);
			const char* component_name = component_type.name();

			if (ImGui::Button(component_name)) {
				entt::meta_func add_comp_fn = type.func(Reflect::add_component_func_name);
				assert(add_comp_fn);
				auto ret = add_comp_fn.invoke({}, entt_registry, entity);
				assert(ret);
			}
		}
	}
}