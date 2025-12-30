#include "editor.h"

#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>

#include "engine/components/components.h"
#include "engine/systems/script_system.h"
#include "editor/hierarchical_panel.h"
#include "editor/load_scene_button.h"
#include "editor/save_scene_button.h"
#include "editor/property_panel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Editor {
	Editor3D::Editor3D(entt::registry& entt_registry,sol::state& sol_state, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer) : entt_registry(&entt_registry),sol_state(&sol_state), hierarchical_panel(entt_registry), component_panel(entt_registry), serializer(serializer) {

	};

	void Editor3D::render() {
		// ImGui::ShowDemoWindow();
		render_top_bar();
		render_save_load_panel();
		hierarchical_panel.render();
		component_panel.render(hierarchical_panel.selected_entity);
		render_imguizmo();
		if (is_scripts_running) {
			Engine::script_system_tick(*entt_registry, *sol_state);
		}
	}

	void Editor3D::render_top_bar()
	{
		ImGui::Begin("Topbar");
		if (!is_scripts_running) {
			if (ImGui::Button("[Run]")) {
				if (is_first_time_running_scripts) {
					Engine::script_system_init(*entt_registry, *sol_state);
				}
				is_first_time_running_scripts = false;
				is_scripts_running = true;
			}
		}
		else {
			if (ImGui::Button("[Pause]")) {
				is_scripts_running = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("[Restart]")) {
			Engine::script_system_reload(*entt_registry, *sol_state);
			Engine::script_system_init(*entt_registry, *sol_state);
			is_scripts_running = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("[Stop]")) {
			Engine::script_system_reload(*entt_registry, *sol_state);
			is_first_time_running_scripts = true;
			is_scripts_running = false;
		}
		ImGui::End();
	}

	void Editor3D::render_imguizmo()
	{
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
		if (ImGui::Button("[LOAD]")) {
			std::ofstream of("scene.kasset");
			{
				serializer.save(*entt_registry, of);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("[SAVE]")) {
			std::ifstream ifs("scene.kasset");
			serializer.load(*entt_registry, ifs);
		}
		ImGui::End();
	}
}