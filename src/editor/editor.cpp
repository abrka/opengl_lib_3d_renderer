#include "editor.h"

#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>

#include "engine/components/components.h"
#include "editor/hierarchical_panel.h"
#include "editor/load_scene_button.h"
#include "editor/save_scene_button.h"
#include "editor/property_panel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Editor {
	Editor3D::Editor3D(entt::registry& entt_registry, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer) : entt_registry(&entt_registry), hierarchical_panel(entt_registry), component_panel(entt_registry), serializer(serializer) {

	};

	void Editor3D::render() {
		ImGui::ShowDemoWindow();

		render_save_load_panel();

		hierarchical_panel.render();

		component_panel.render(hierarchical_panel.selected_entity);

		// draw gizmos with imguizmo

		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

		auto [screen_width, screen_height] = ImGui::GetMainViewport()->Size;
		ImGuizmo::SetRect(0, 0, (float)screen_width, (float)screen_height);

		auto selected_entity = hierarchical_panel.selected_entity;
		if (!selected_entity.has_value()) {
			return;
		}
		auto* transform_comp = entt_registry->try_get<Engine::TransformComponent>(selected_entity.value());
		if (!transform_comp) {
			return;
		}
		auto entt_view_camera = entt_registry->view<const Engine::CameraComponent>();

		for (auto [entity, camera_component] : entt_view_camera.each()) {
			auto& camera = camera_component.camera;
			ImGuizmo::Manipulate(glm::value_ptr(camera.get_view_matrix()), glm::value_ptr(camera.get_projection_matrix()), ImGuizmo::OPERATION::UNIVERSAL, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform_comp->transform));
		}
	}

	void Editor3D::render_save_load_panel() {
		ImGui::Begin("Serialize");
		SaveSceneButton::render("scene.kasset", "Save Scene", *entt_registry, serializer);
		ImGui::SameLine();
		LoadSceneButton::render("scene.kasset", "Load Scene", *entt_registry, serializer);
		ImGui::End();
	}
}