#pragma once

#include <glm/glm.hpp>


namespace Renderer {
	struct Camera {
		glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
		glm::mat4 orientation = glm::mat4(1.0);
		double fov = 45;
		double near_plane_dist = 0.1;
		double far_plane_dist = 100.0;
		double aspect_ratio = 1.0f;

		glm::mat4 get_projection_matrix() const;
		glm::mat4 get_view_matrix() const;

	};
}