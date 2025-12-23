#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp> // For eulerAngles

#include <imgui.h>
#include <ImReflect.hpp>


IMGUI_REFLECT(Engine::TransformComponent, transform)
IMGUI_REFLECT(Engine::NameComponent, name)
IMGUI_REFLECT(Renderer::Camera, position, orientation, fov, near_plane_dist, far_plane_dist, aspect_ratio)
IMGUI_REFLECT(Engine::CameraComponent, camera)
IMGUI_REFLECT(Renderer::PointLight, color, ambient_strength, specular_strength)
IMGUI_REFLECT(Engine::PointLightComponent, light)

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

	value = glm::recompose(scale, glm::quat(glm::radians(rotation_euler_angle)), translation, skew, perspective);

	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(t_response);
}