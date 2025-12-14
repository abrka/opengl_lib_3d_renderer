#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include "renderer/renderer.h"

#include "editor/hierarchical_panel.h"
#include "editor/load_scene_button.h"
#include "editor/save_scene_button.h"

namespace Editor {
	class Editor {
	public:

		Editor(Renderer::Renderer3D& renderer, entt::registry& entt_registry) : renderer(&renderer), entt_registry(&entt_registry), hierarchical_panel(entt_registry) {};
		
		void render() {
			ImGui::ShowDemoWindow();

			ImGui::Begin("Serialize");
			SaveSceneButton::render("scene.kasset", "Save Scene", *entt_registry);
			ImGui::SameLine();
			LoadSceneButton::render("scene.kasset", "Load Scene", *entt_registry);
			ImGui::End();

			hierarchical_panel.render();


			// draw gizmos with imguizmo

			ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

			auto [screen_width, screen_height] = renderer->get_screen_width_and_height();
			ImGuizmo::SetRect(0, 0, screen_width, screen_height);

			auto selected_entity = hierarchical_panel.selected_entity;
			if (selected_entity == entt::null) {
				return;
			}
			auto* transform_comp = entt_registry->try_get<Engine::TransformComponent>(selected_entity);
			if (!transform_comp) {
				return;
			}
			auto& camera = renderer->render_ctx.cam;
			ImGuizmo::Manipulate(glm::value_ptr(camera.get_view_matrix()), glm::value_ptr(camera.get_projection_matrix()), imguizmo_operation, imguizmo_mode, glm::value_ptr(transform_comp->transform));

		}
	private:
		entt::registry* entt_registry;
		Renderer::Renderer3D* renderer{};
		HierarchicalPanel hierarchical_panel{};
		ImGuizmo::OPERATION imguizmo_operation{ ImGuizmo::OPERATION::UNIVERSAL };
		ImGuizmo::MODE imguizmo_mode{ ImGuizmo::MODE::LOCAL };
	};
}