#pragma once

#include <glm/glm.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Math.h>

JPH::Vec3 glm_vec3_to_jph_vec3(const glm::vec3& glm_vec3);
JPH::Vec4 glm_vec4_to_jph_vec4(const glm::vec4& glm_vec4);
JPH::Quat glm_quat_to_jph_quat(const glm::quat& glm_quat);
glm::vec3 jph_vec3_to_glm_vec3(const JPH::RVec3& jph_vec3);
glm::quat jph_quat_to_glm_quat(const JPH::Quat& jph_quat);
glm::mat4 jph_mat4_to_glm_mat4(const JPH::RMat44& jph_mat4);
JPH::Mat44 glm_mat4_to_jph_mat4(const glm::mat4& glm_mat4);