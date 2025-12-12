#include "renderer/renderer.h"
#include "engine/entity/entity.h"

namespace Editor {
	class HierarchicalPanel
	{
	public:

		Engine::Entity* selected_entity{};

		HierarchicalPanel(entt::registry& entt_registry) : entt_registry(&entt_registry) {}

		void render(Engine::Entity& node) {
			render_add_button();
			ImGui::SameLine();
			render_remove_button();
			render_entities(node);
		}
	private:
		entt::registry* entt_registry{};
		size_t added_entity_count{};

		void render_add_button() {
			if (ImGui::Button("[+] Add Entity")) {
				if (selected_entity) {
					auto entity = std::make_unique<Engine::Entity>(*entt_registry);
					entity->name = "Entity " + std::to_string(added_entity_count);
					selected_entity->add_child(std::move(entity));
					added_entity_count++;
				}
			}
		}
		void render_remove_button() {
			if (ImGui::Button("[-] Remove Entity")) {
				if (selected_entity) {
					bool success = selected_entity->destroy();
					if (success) {
						selected_entity = nullptr;
					}
				}
			}
		}
		void render_entities(Engine::Entity& node) {
			ImGuiTreeNodeFlags flags{ImGuiTreeNodeFlags_DefaultOpen};
			if (&node == selected_entity) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			if (node.children.empty()) {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			bool is_open = ImGui::TreeNodeEx(&node, flags, node.name.c_str());
			if (ImGui::IsItemClicked()) {
				selected_entity = &node;
			}
			if (is_open) {
				for (size_t i = 0; i < node.children.size(); i++)
				{
					ImGuiTreeNodeFlags child_flags{};
					render_entities(*node.children[i]);
				}
				ImGui::TreePop();
			}
		}
	};
}
