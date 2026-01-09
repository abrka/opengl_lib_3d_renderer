#pragma once

#include <optional>
#include <string_view>
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <entt/fwd.hpp>
#include <imgui.h>
#include "renderer/camera.h"


namespace Editor {
    
	class JoltDebugRenderer : public JPH::DebugRendererSimple {
    public:
        JoltDebugRenderer(entt::registry& entt_registry);
        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
        virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
    private:
        entt::registry* entt_registry{};
        Renderer::Camera* get_camera();
        std::optional<ImVec2> transform_jph_point_to_imgui_point(const JPH::Vec3& jph_point);
    };
}