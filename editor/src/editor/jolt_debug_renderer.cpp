#include "jolt_debug_renderer.h"

#include <iostream>
#include <array>
#include <span>
#include <string_view>
#include <glm/glm.hpp>
#include <imgui.h>
#include <entt/entt.hpp>
#include <GL3D/mesh.h>
#include <GL3D/shader_program.h>

#include "renderer/shader_builder.h"
#include "engine/components/camera_component.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace Editor {
	static glm::vec3 jph_vec3_to_glm_vec3(const JPH::Vec3& jph_vec3) {
		return glm::vec3{ jph_vec3.GetX(), jph_vec3.GetY(), jph_vec3.GetZ() };
	}
	static glm::vec4 jph_color_to_glm_vec4(const JPH::Color& jph_color) {
		return glm::vec4{ jph_color.r, jph_color.g, jph_color.b, jph_color.a };
	}

	JoltDebugRenderer::JoltDebugRenderer(entt::registry& entt_registry) : DebugRendererSimple(), entt_registry(&entt_registry)
	{
	}

	void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
	{
		auto* draw_list = ImGui::GetBackgroundDrawList();
		auto v1 = transform_jph_point_to_imgui_point(inFrom);
		auto v2 = transform_jph_point_to_imgui_point(inTo);
		if (!v1 || !v2) {
			return;
		}
		draw_list->AddLine(*v1, *v2, inColor.GetUInt32());
	}
	void JoltDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow)
	{
		auto* draw_list = ImGui::GetBackgroundDrawList();
		auto v1 = transform_jph_point_to_imgui_point(inV1);
		auto v2 = transform_jph_point_to_imgui_point(inV2);
		auto v3 = transform_jph_point_to_imgui_point(inV3);
		if (!v1 || !v2 || !v3) {
			return;
		}
		draw_list->AddTriangleFilled(*v1, *v2, *v3, inColor.GetUInt32());
	}

	void JoltDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight)
	{
		auto* draw_list = ImGui::GetBackgroundDrawList();
		auto pos = transform_jph_point_to_imgui_point(inPosition);
		if (!pos) {
			return;
		}
		draw_list->AddText(*pos, inColor.GetUInt32(), inString.data());
	}
	Renderer::Camera* JoltDebugRenderer::get_camera()
	{
		auto entt_view_camera = entt_registry->view<Engine::CameraComponent>();
		for (auto [entity, cam] : entt_view_camera.each()) {
			return &cam.camera;
		}
		return nullptr;
	}
	std::optional<ImVec2> JoltDebugRenderer::transform_jph_point_to_imgui_point(const JPH::Vec3& jph_point)
	{
		glm::vec3 point = jph_vec3_to_glm_vec3(jph_point);
		auto* camera = get_camera();
		if (!camera) {
			return std::nullopt;
		}

		glm::vec4 point_clip_space = camera->get_projection_matrix() * camera->get_view_matrix() * glm::vec4{ point, 1.0f };
		glm::vec4 point_ndc = point_clip_space / point_clip_space.w;
		if (point_ndc.z < -1.0f || point_ndc.z > 1.0f) {
			return std::nullopt;
		}

		auto [screen_width, screen_height] = ImGui::GetMainViewport()->Size;

		float x = (point_ndc.x + 1.0f) * 0.5f * screen_width;
		float y = (1.0f - point_ndc.y) * 0.5f * screen_height;

		return ImVec2{ x,y };
	}
}