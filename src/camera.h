#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {

	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
	glm::mat4 orientation = glm::mat4(1.0);
	double fov = 45;
	double near_plane_dist = 0.1;
	double far_plane_dist = 100.0;

	glm::mat4 get_projection_matrix(double screen_width, double screen_height) {
		return glm::perspective(glm::radians(fov), (screen_width / screen_height), near_plane_dist, far_plane_dist);
	}

	glm::mat4 get_view_matrix() {
		glm::vec3 eye = position;
		glm::vec3 forward = orientation[2];
		glm::vec3 center = position + forward;
		glm::vec3 up = orientation[1];
		glm::mat4 view = glm::lookAt(eye, center, up);
		return view;
	}

};