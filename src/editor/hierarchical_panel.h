#pragma once

#include <optional>

#include <entt/entt.hpp>
#include <imgui.h>


namespace Editor {
	class HierarchicalPanel
	{
	public:
		std::optional<entt::entity> selected_entity{ std::nullopt };

		HierarchicalPanel() = default;
		HierarchicalPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {}

		void render() {
			ImGui::Begin("Entities");
			render_add_button();
			ImGui::SameLine();
			render_remove_button();
			entt::entity root_entity = entt_registry->view<Engine::RootComponent>().front();
			if (entt_registry->valid(root_entity)) {
				render_entities(root_entity);
			}
			else {
				std::cout << "EDITOR::WARNING entt registry does not contain an entity with RootComponent. Can't render hierarchial panel" << "\n";
			}
			ImGui::End();
		}
	private:
		entt::registry* entt_registry{};
		size_t added_entity_count{};

		void render_add_button() {
			if (ImGui::Button("[+] Add Entity")) {
				if (!selected_entity.has_value()) {
					return;
				}
				auto entity = entt_registry->create();
				entt_registry->emplace<Engine::NameComponent>(entity, "Entity " + std::to_string(added_entity_count));
				Engine::add_child(*entt_registry, selected_entity.value(), entity);
				added_entity_count++;
			}
		}
		void render_remove_button() {
			if (ImGui::Button("[-] Remove Entity")) {
				if (!selected_entity.has_value()) {
					return;
				}
				Engine::destroy_entity(*entt_registry, selected_entity.value());
				selected_entity.reset();
			}
		}
		void render_entities(const entt::entity entity) {
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
	};
}
