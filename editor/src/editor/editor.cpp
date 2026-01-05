#include "editor.h"

#include <istream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include "engine/components/components.h"
#include "engine/systems/systems.h"
#include "editor/hierarchical_panel.h"
#include "editor/property_panel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Editor {
	Editor3D::Editor3D(entt::registry& entt_registry, sol::state& sol_state, Physics::World& physics_world, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer) : entt_registry(&entt_registry), sol_state(&sol_state), physics_world(&physics_world), hierarchical_panel(entt_registry), component_panel(entt_registry), serializer(serializer) {

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
		if (is_physics_running) {
			Engine::physics_body_system_tick(*entt_registry);
			physics_world->tick(1.0f / 60.0f);
		}
	}

	void Editor3D::render_top_bar()
	{
		ImGui::Begin("Topbar");
		if (!is_scripts_running) {
			if (ImGui::Button("[Run]")) {
				Engine::script_system_init(*entt_registry);
				is_scripts_running = true;
				is_physics_running = true;
			}
		}
		else {
			if (ImGui::Button("[Pause]")) {
				is_scripts_running = false;
				is_physics_running = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("[Restart]")) {
			// TODO: first we need to reset the scene by loading it from file again
			Engine::script_system_init(*entt_registry);
			is_scripts_running = true;
			is_physics_running = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("[Stop]")) {
			// TODO: first we need to reset the scene by loading it from file again
			is_scripts_running = false;
			is_physics_running = false;
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

	void Editor3D::render_save_button()
	{
		if (ImGui::Button("Save")) {
			const char* filepath_c_str = tinyfd_saveFileDialog("Save scene to file", "", 0, NULL, NULL);
			if (!filepath_c_str) {
				return;
			}
			std::string filepath_str{ filepath_c_str };
			std::ofstream of(filepath_str);
			if (!of) {
				return;
			}
			{
				serializer.save(*entt_registry, of);
			}
		}
	}

	void Editor3D::render_load_button()
	{
		if (ImGui::Button("Load")) {
			const char* filepath_c_str = tinyfd_openFileDialog("Load scene from file", "", 0, NULL, NULL, 0);
			if (!filepath_c_str) {
				return;
			}
			std::string filepath_str{ filepath_c_str };
			std::ifstream ifs(filepath_str);
			if (!ifs) {
				return;
			}
			serializer.load(*entt_registry, ifs);
		}
	}

	void Editor3D::render_save_load_panel() {
		ImGui::Begin("Serialize");
		render_save_button();
		ImGui::SameLine();
		render_load_button();
		ImGui::End();
	}
}