#include "renderer/renderer.h"


namespace Editor {
	template<typename T>
	class HierarchialPanel
	{
	public:
		T* selected_node{};

		void render(T& node) {
			ImGuiTreeNodeFlags flags{};
			if (&node == selected_node) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			if (node.children.empty()) {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			bool is_open = ImGui::TreeNodeEx(&node, flags, node.name.c_str());
			if (ImGui::IsItemClicked()) {
				selected_node = &node;
			}
			if (is_open) {
				for (size_t i = 0; i < node.children.size(); i++)
				{
					ImGuiTreeNodeFlags child_flags{};
					render(*node.children[i]);
				}
				ImGui::TreePop();
			}
		}
	};
}
