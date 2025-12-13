#pragma once

#include <entt/entt.hpp>
#include "renderer/renderer.h"


namespace Editor {
	class HierarchicalPanel
	{
	public:
		entt::entity selected_entity{ entt::null };

		HierarchicalPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {}

		void render(entt::entity root_entity) {
			render_add_button();
			ImGui::SameLine();
			render_remove_button();
			if (entt_registry->valid(root_entity)) {
				render_entities(root_entity);
			}
		}
	private:
		entt::registry* entt_registry{};
		size_t added_entity_count{};

		void render_add_button() {
			if (ImGui::Button("[+] Add Entity")) {
				if (!entt_registry->valid(selected_entity)) {
					return;
				}
				auto entity = entt_registry->create();
				entt_registry->emplace<Engine::NameComponent>(entity, "Entity " + std::to_string(added_entity_count));
				Engine::add_child(*entt_registry, selected_entity, entity);
				added_entity_count++;
			}
		}
		void render_remove_button() {
			if (ImGui::Button("[-] Remove Entity")) {
				if (entt_registry->valid(selected_entity)) {
					Engine::destroy_entity(*entt_registry, selected_entity);
					selected_entity = entt::null;
				}
			}
		}
		void render_entities(const entt::entity entity) {
			ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_DefaultOpen };
			if (entity == selected_entity) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			auto children = entt_registry->get_or_emplace<Engine::ChildrenComponent>(entity);
			if (children.children.empty()) {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			auto& name = entt_registry->get_or_emplace<Engine::NameComponent>(entity);
			bool is_open = ImGui::TreeNodeEx((void*)entity, flags, name.name.c_str());
			if (ImGui::IsItemClicked()) {
				selected_entity = entity;
			}
			if (is_open) {
				for (const auto& child : children.children) {
					render_entities(child);
				}
				ImGui::TreePop();
			}
		}
	};
}
