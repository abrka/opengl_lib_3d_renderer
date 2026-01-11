#include "editor.h"

#include <istream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include "engine/components/components.h"
#include "engine/systems/systems.h"
#include "editor/hierarchical_panel.h"
#include "editor/property_panel.h"
#include "utils/jph_math_to_glm_math.h"

namespace Editor {
	Editor3D::Editor3D(entt::registry& entt_registry, sol::state& sol_state, Physics::World& physics_world, Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive>& serializer) : entt_registry(&entt_registry), sol_state(&sol_state), physics_world(&physics_world), hierarchical_panel(entt_registry), component_panel(entt_registry), serializer(serializer), jolt_debug_renderer(entt_registry) {

	};

	void Editor3D::render() {
		// ImGui::ShowDemoWindow();
		render_top_bar();
		render_save_load_panel();
		hierarchical_panel.render();
		component_panel.render(hierarchical_panel.selected_entity);
		render_imguizmo();
		
		bool is_physics_running = (game_state == GameState::RUNNING);
		bool is_scripts_running = (game_state == GameState::RUNNING);

		if (!is_physics_running) {
			auto entt_view_physics_bodies = entt_registry->view<Engine::PhysicsBodyInfoComponent, Engine::TransformComponent>();
			for (auto entity : entt_view_physics_bodies) {
				render_jolt_shape_of_entity(entity);
			}
		}

		if (is_scripts_running) {
			Engine::script_system_tick(*entt_registry, *sol_state);
		}
		if (is_physics_running) {
			Engine::physics_body_system_tick(*entt_registry);
			physics_world->tick(1.0f / 60.0f);
		}
		if (is_physics_running || (game_state == GameState::PAUSED)) {
			render_jolt_debug();
		}
	}

	void Editor3D::render_jolt_shape_of_entity(entt::entity entity)
	{
		Engine::TransformComponent* transform_comp = entt_registry->try_get<Engine::TransformComponent>(entity);
		if (!transform_comp) {
			return;
		}
		Engine::PhysicsBodyInfoComponent* physics_body_info = entt_registry->try_get<Engine::PhysicsBodyInfoComponent>(entity);
		if (!physics_body_info) {
			return;
		}
		JPH::Mat44 transform = glm_mat4_to_jph_mat4(transform_comp->transform);

		assert(physics_body_info->shape_settings);
		physics_body_info->shape_settings->ClearCachedResult();
		auto shape_result = physics_body_info->shape_settings->Create();
		if (shape_result.IsValid()) {
			shape_result.Get()->Draw(&jolt_debug_renderer, transform, JPH::Vec3{ 1.0f,1.0f,1.0f }, JPH::Color::sRed, true, true);
		}
	}

	void Editor3D::render_top_bar()
	{
		ImGui::Begin("Topbar");
		if (game_state != GameState::RUNNING) {
			if (ImGui::Button("[Run]")) {
				if (game_state != GameState::PAUSED) {
					reload_scene_from_last_saved_file();
				}
				Engine::script_system_init(*entt_registry);
				game_state = GameState::RUNNING;
			}
		}
		else {
			if (ImGui::Button("[Pause]")) {
				game_state = GameState::PAUSED;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("[Restart]")) {
			reload_scene_from_last_saved_file();
			Engine::script_system_init(*entt_registry);
			game_state = GameState::RUNNING;
		}
		ImGui::SameLine();
		render_stop_scene_button();
		ImGui::End();
	}

	void Editor3D::render_stop_scene_button()
	{
		if (ImGui::Button("[Stop]")) {
			reload_scene_from_last_saved_file();
			game_state = GameState::STOPPED;
		}
	}

	void Editor3D::reload_scene_from_last_saved_file()
	{
		if (!last_saved_scene) {
			save_scene_popup();
		}
		std::ifstream ifs(*last_saved_scene);
		if (!ifs) {
			return;
		}
		serializer.load(*entt_registry, ifs);
	}

	void Editor3D::save_scene_to_last_saved_file()
	{
		if (!last_saved_scene) {
			save_scene_popup();
		}
		std::ofstream of(*last_saved_scene);
		if (!of) {
			return;
		}
		{
			serializer.save(*entt_registry, of);
		}
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
	static JPH::Vec3 glm_vec3_to_jph_vec3(const glm::vec3& glm_vec3) {
		return JPH::Vec3{ glm_vec3.x, glm_vec3.y, glm_vec3.z };
	}

	void Editor3D::render_jolt_debug()
	{
		JPH::BodyManager::DrawSettings settings{};
		settings.mDrawBoundingBox = true;
		settings.mDrawCenterOfMassTransform = true;
		settings.mDrawShape = true;
		settings.mDrawShapeWireframe = true;
		settings.mDrawVelocity = true;
		settings.mDrawSleepStats = true;
		settings.mDrawVelocity = true;
		settings.mDrawWorldTransform = true;

		auto entt_view_camera = entt_registry->view<const Engine::CameraComponent>();
		for (auto [entity, camera_component] : entt_view_camera.each()) {
			auto& camera = camera_component.camera;
			jolt_debug_renderer.SetCameraPos(glm_vec3_to_jph_vec3(camera.position));
		};
		physics_world->jph_physics_system->DrawBodies(settings, &jolt_debug_renderer);
		jolt_debug_renderer.NextFrame();
	}

	void Editor3D::render_save_button()
	{
		if (ImGui::Button("Save")) {
			save_scene_popup();
		}
	}

	void Editor3D::save_scene_popup()
	{
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
		last_saved_scene = filepath_str;
		game_state = GameState::STOPPED;
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
			last_saved_scene = filepath_str;
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