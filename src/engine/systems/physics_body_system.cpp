#include "physics_body_system.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include "physics_wrapper/world.h"
#include "physics_wrapper/body.h"
#include "engine/components/physics_body_info_component.h"
#include "engine/components/physics_body_component.h"
#include "engine/components/transform_component.h"
#include "utils/jph_math_to_glm_math.h"

namespace Engine {
	void physics_body_load_system(entt::registry& entt_registry, Physics::World& physics_world)
	{
		auto entt_view_bodies = entt_registry.view<Engine::PhysicsBodyInfoComponent, Engine::TransformComponent>();
		for (auto [entity, body_info, transform] : entt_view_bodies.each()) {
			if (!body_info.requires_reload) {
				continue;
			}
			glm::vec3 scale{};
			glm::quat rotation{};
			glm::vec3 translation{};
			glm::vec3 skew{};
			glm::vec4 perspective{};
			glm::decompose(transform.transform, scale, rotation, translation, skew, perspective);
			JPH::Vec3 jph_position{ glm_vec3_to_jph_vec3(translation) };
			JPH::Quat jph_rotation{ glm_quat_to_jph_quat(rotation) };

			assert(body_info.shape_settings);
			body_info.shape_settings->ClearCachedResult();
			auto shape = body_info.shape_settings->Create();
			if (shape.HasError()) {
				std::cout << "[ERROR][ENGINE][PHYSICS BODY SYSTEM]: " << shape.GetError() << "\n";
				continue;
			}
			JPH::BodyCreationSettings settings{ shape.Get(),jph_position,jph_rotation, body_info.motion_type, body_info.layer };
			auto body = std::make_unique<Physics::Body>(physics_world, settings);
			entt_registry.emplace_or_replace<Engine::PhysicsBodyComponent>(entity, Engine::PhysicsBodyComponent{ std::move(body) });
			body_info.requires_reload = false;
		}
	}
	void physics_body_system_tick(entt::registry& entt_registry)
	{
		auto entt_view_bodies = entt_registry.view<Engine::PhysicsBodyComponent, Engine::TransformComponent>();
		for (auto [entity, body, transform] : entt_view_bodies.each()) {
			auto jph_position = body.body->jph_body->GetPosition();
			auto position = jph_vec3_to_glm_vec3(jph_position);
			auto jph_rotation = body.body->jph_body->GetRotation();
			auto rotation = jph_quat_to_glm_quat(jph_rotation);

			auto new_transform = glm::translate(glm::toMat4(rotation), position);
			transform.transform = new_transform;
		}
	}
}