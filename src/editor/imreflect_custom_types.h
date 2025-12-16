#pragma once

#include <imgui.h>
#include <ImReflect.hpp>


void tag_invoke(ImReflect::ImInput_t, const char* label, glm::mat4& value, ImSettings& settings, ImResponse& response) {
	auto& mat4_response = response.get<glm::mat4>();

	bool changed = false;
	changed |= ImGui::InputFloat4(std::string(std::string(label) + " 0").c_str(), glm::value_ptr(value[0]));
	changed |= ImGui::InputFloat4(std::string(std::string(label) + " 1").c_str(), glm::value_ptr(value[1]));
	changed |= ImGui::InputFloat4(std::string(std::string(label) + " 2").c_str(), glm::value_ptr(value[2]));
	changed |= ImGui::InputFloat4(std::string(std::string(label) + " 3").c_str(), glm::value_ptr(value[3]));
	if (changed) mat4_response.changed();

	/* Check hovered, activated, etc*/
	ImReflect::Detail::check_input_states(mat4_response);
}