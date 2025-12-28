#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer {
	glm::mat4 Camera::get_projection_matrix() const {
		return glm::perspective(glm::radians(fov), aspect_ratio, near_plane_dist, far_plane_dist);
	}

	glm::mat4 Camera::get_view_matrix() const {
		glm::vec3 eye = position;
		glm::vec3 forward = orientation[2];
		glm::vec3 center = position + forward;
		glm::vec3 up = orientation[1];
		glm::mat4 view = glm::lookAt(eye, center, up);
		return view;
	}
}