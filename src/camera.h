#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

public:
	glm::vec3 position{ 0.0f };
	glm::vec3 direction{ 0.0f , 0.0f , -1.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	glm::mat4 get_projection_matrix(float screen_width, float screen_height,float fov=45.0, float near_plane_dist = 0.1, float far_plane_dist = 100.0) {
		return glm::perspective(glm::radians(fov), (screen_width / screen_height), near_plane_dist, far_plane_dist);
	}

	glm::mat4 get_view_matrix() {
		return glm::lookAt(position, position + direction, up);
	}

};