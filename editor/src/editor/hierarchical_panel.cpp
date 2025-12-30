#include "hierarchical_panel.h"

#include <entt/entt.hpp>
#include "engine/components/components.h"

namespace Editor {
	HierarchicalPanel::HierarchicalPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {}

	void HierarchicalPanel::render() {
		ImGui::Begin("Entities");
		render_add_button();
		ImGui::SameLine();
		render_remove_button();
		entt::entity root_entity = entt_registry->view<Engine::RootComponent>().front();
		if (root_entity != entt::null) {
			render_entities(root_entity);
		}
		ImGui::End();
	}

	void HierarchicalPanel::render_add_button() {
		if (ImGui::Button("[+] Add Entity")) {
			entt::entity which_entity_to_add_to{ entt::null };
			if (selected_entity.has_value()) {
				which_entity_to_add_to = selected_entity.value();
			}
			else {
				entt::entity root_entity = entt_registry->view<Engine::RootComponent>().front();
				if (root_entity == entt::null) {
					return;
				}
				which_entity_to_add_to = root_entity;
			}
			auto entity = entt_registry->create();
			entt_registry->emplace<Engine::NameComponent>(entity, "Entity " + std::to_string(added_entity_count));
			Engine::add_child(*entt_registry, which_entity_to_add_to, entity);
			added_entity_count++;
		}
	}
	void HierarchicalPanel::render_remove_button() {
		if (ImGui::Button("[-] Remove Entity")) {
			if (!selected_entity.has_value()) {
				return;
			}
			Engine::destroy_entity(*entt_registry, selected_entity.value());
			selected_entity.reset();
		}
	}
	void HierarchicalPanel::render_entities(entt::entity entity) {
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_DefaultOpen };
		if (entity == selected_entity) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		auto* children = entt_registry->try_get<Engine::ChildrenComponent>(entity);
		if (!children || (children && children->children.empty())) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		auto* name = entt_registry->try_get<Engine::NameComponent>(entity);
		std::string output_name{ "Entity does not contain a NameComponent" };
		if (name) {
			output_name = name->name;
		}
		bool is_open = ImGui::TreeNodeEx((void*)entity, flags, output_name.c_str());
		if (ImGui::IsItemClicked()) {
			selected_entity = entity;
		}
		if (is_open) {
			if (children) {
				for (const auto& child : children->children) {
					render_entities(child);
				}
			}
			ImGui::TreePop();
		}
	}
}