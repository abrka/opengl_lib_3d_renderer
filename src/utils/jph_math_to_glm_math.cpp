#include "jph_math_to_glm_math.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Jolt/Math/Math.h>

JPH::Vec3 glm_vec3_to_jph_vec3(const glm::vec3& glm_vec3) {
	return JPH::Vec3{ glm_vec3.x, glm_vec3.y, glm_vec3.z };
}
JPH::Vec4 glm_vec4_to_jph_vec4(const glm::vec4& glm_vec4) {
	return JPH::Vec4{ glm_vec4.x, glm_vec4.y, glm_vec4.z, glm_vec4.w };
}
JPH::Quat glm_quat_to_jph_quat(const glm::quat& glm_quat) {
	return JPH::Quat{ glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w };
};
glm::vec3 jph_vec3_to_glm_vec3(const JPH::RVec3& jph_vec3) {
	return glm::vec3{ jph_vec3.GetX(), jph_vec3.GetY(), jph_vec3.GetZ() };
}
glm::quat jph_quat_to_glm_quat(const JPH::Quat& jph_quat) {
	return glm::quat{ jph_quat.GetX(), jph_quat.GetY(), jph_quat.GetZ(), jph_quat.GetW() };
};

glm::mat4 jph_mat4_to_glm_mat4(const JPH::RMat44& jph_mat4)
{
	return glm::mat4{
		jph_mat4(0,0), jph_mat4(0,1), jph_mat4(0,2), jph_mat4(0,3),
		jph_mat4(1,0), jph_mat4(1,1), jph_mat4(1,2), jph_mat4(1,3),
		jph_mat4(2,0), jph_mat4(2,1), jph_mat4(2,2), jph_mat4(2,3),
		jph_mat4(3,0), jph_mat4(3,1), jph_mat4(3,2), jph_mat4(3,3),
	};
}
JPH::Mat44 glm_mat4_to_jph_mat4(const glm::mat4& glm_mat4)
{
	return JPH::Mat44{
		glm_vec4_to_jph_vec4(glm_mat4[0]),
		glm_vec4_to_jph_vec4(glm_mat4[1]),
		glm_vec4_to_jph_vec4(glm_mat4[2]),
		glm_vec4_to_jph_vec4(glm_mat4[3])
	};
}