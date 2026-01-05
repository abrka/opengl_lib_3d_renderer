#include "physics_body_system.h"

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

namespace Engine {
	static JPH::Vec3 glm_vec3_to_jph_vec3(const glm::vec3& glm_vec3) {
		return JPH::Vec3{ glm_vec3.x, glm_vec3.y, glm_vec3.z };
	}
	static JPH::Quat glm_quat_to_jph_quat(const glm::quat& glm_quat) {
		return JPH::Quat{ glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w };
	};
	static glm::vec3 jph_vec3_to_glm_vec3(const JPH::RVec3& jph_vec3) {
		return glm::vec3{ jph_vec3.GetX(), jph_vec3.GetY(), jph_vec3.GetZ()};
	}
	static glm::quat jph_quat_to_glm_quat(const JPH::Quat& jph_quat) {
		return glm::quat{ jph_quat.GetX(), jph_quat.GetY(), jph_quat.GetZ(), jph_quat.GetW()};
	};

	static glm::mat4 jph_mat4_to_glm_mat4(const JPH::RMat44& jph_mat4)
	{
		return glm::mat4{
			jph_mat4(0,0), jph_mat4(0,1), jph_mat4(0,2), jph_mat4(0,3),
			jph_mat4(1,0), jph_mat4(1,1), jph_mat4(1,2), jph_mat4(1,3),
			jph_mat4(2,0), jph_mat4(2,1), jph_mat4(2,2), jph_mat4(2,3),
			jph_mat4(3,0), jph_mat4(3,1), jph_mat4(3,2), jph_mat4(3,3),
		};
	}

	void physics_body_load_system(entt::registry& entt_registry, Physics::World& physics_world)
	{
		auto entt_view_bodies = entt_registry.view<Engine::PhysicsBodyInfoComponent, Engine::TransformComponent>(entt::exclude<Engine::PhysicsBodyComponent>);
		for (auto [entity, body_info, transform] : entt_view_bodies.each()) {
			if (!body_info.shape_settings) {
				continue;
			}
			glm::vec3 scale{};
			glm::quat rotation{};
			glm::vec3 translation{};
			glm::vec3 skew{};
			glm::vec4 perspective{};
			glm::decompose(transform.transform, scale, rotation, translation, skew, perspective);
			JPH::Vec3 jph_position{glm_vec3_to_jph_vec3(translation)};
			JPH::Quat jph_rotation{glm_quat_to_jph_quat(rotation)};
			JPH::BodyCreationSettings settings{ body_info.shape_settings.get(),jph_position,jph_rotation, body_info.motion_type, body_info.layer};
			auto body = std::make_unique<Physics::Body>(physics_world, settings);
			entt_registry.emplace_or_replace<Engine::PhysicsBodyComponent>(entity, Engine::PhysicsBodyComponent{ std::move(body) });
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