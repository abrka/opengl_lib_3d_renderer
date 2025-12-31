#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp> // For eulerAngles
#include <imgui.h>
#include <ImReflect.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "engine/components/components.h"
#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"

IMGUI_REFLECT(Engine::TransformComponent, transform)
IMGUI_REFLECT(Engine::NameComponent, name)
IMGUI_REFLECT(Renderer::Camera, position, orientation, fov, near_plane_dist, far_plane_dist, aspect_ratio)
IMGUI_REFLECT(Engine::CameraComponent, camera)
IMGUI_REFLECT(Renderer::PointLight, color, ambient_strength, diffuse_strength, specular_strength)
IMGUI_REFLECT(Engine::PointLightComponent, light)
IMGUI_REFLECT(Engine::ScriptInfoComponent, filepath)
IMGUI_REFLECT(Engine::MeshInfoComponent, filepath)
IMGUI_REFLECT(Engine::ShaderInfoComponent, vertex_filepath, fragment_filepath)
IMGUI_REFLECT(AssetBuilder::Material, uniforms_int, uniforms_float, uniforms_vec3, uniforms_mat4)
IMGUI_REFLECT(Engine::MaterialComponent, material)


void tag_invoke(ImReflect::ImInput_t, const char* label, std::filesystem::path& value, ImSettings& settings, ImResponse& response) {
	auto& t_response = response.get<std::filesystem::path>();
	bool changed = false;
	ImGui::PushID(label);
	if (ImGui::ButtonEx("[Load]")) {
		const char* new_filepath_c_str = tinyfd_openFileDialog("Load File", "", 0, NULL, NULL, 0);
		if (new_filepath_c_str) {
			std::string new_filepath_str{ new_filepath_c_str };
			std::filesystem::path new_filepath{ new_filepath_str };
			value = new_filepath;
			changed = true;
		}
	}
	ImGui::PopID();
	ImGui::SameLine();
	std::string new_filepath_str = value.string();
	ImGui::InputText(label, &new_filepath_str);
	if (changed) { t_response.changed(); }
	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(t_response);
}
void tag_invoke(ImReflect::ImInput_t, const char* label, Renderer::color_t& value, ImSettings& settings, ImResponse& response) {
	auto& t_response = response.get<glm::mat4>();
	bool changed = ImGui::ColorEdit3(label, glm::value_ptr(value.color));
	if (changed) { t_response.changed(); }

	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(t_response);
}
void tag_invoke(ImReflect::ImInput_t, const char* label, glm::vec3& value, ImSettings& settings, ImResponse& response) {
	auto& t_response = response.get<glm::mat4>();

	bool changed = ImGui::DragFloat3(label, glm::value_ptr(value));
	if (changed) { t_response.changed(); }

	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(t_response);
}
void tag_invoke(ImReflect::ImInput_t, const char* label, glm::mat4& value, ImSettings& settings, ImResponse& response) {
	auto& t_response = response.get<glm::mat4>();

	bool changed = false;

	glm::vec3 scale{};
	glm::quat rotation{};
	glm::vec3 translation{};
	glm::vec3 skew{};
	glm::vec4 perspective{};
	glm::decompose(value, scale, rotation, translation, skew, perspective);

	changed |= ImGui::DragFloat3("translation", glm::value_ptr(translation));
	auto rotation_euler_angle = glm::degrees(glm::eulerAngles(rotation));
	changed |= ImGui::DragFloat3("rotation", glm::value_ptr(rotation_euler_angle));
	changed |= ImGui::DragFloat3("scale", glm::value_ptr(scale));
	if (changed) { t_response.changed(); }

	if (std::abs(scale.x) < 0.01) {
		scale.x = 0.01;
	}
	if (std::abs(scale.y) < 0.01) {
		scale.y = 0.01;
	}
	if (std::abs(scale.z) < 0.01) {
		scale.z = 0.01;
	}
	value = glm::recompose(scale, glm::quat(glm::radians(rotation_euler_angle)), translation, skew, perspective);

	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(t_response);
}