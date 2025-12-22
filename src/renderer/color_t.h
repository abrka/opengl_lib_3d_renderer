#pragma once

#include <glm/glm.hpp>

namespace Renderer {
	struct color_t {
		glm::vec3 color{};

		color_t() = default;
		color_t(glm::vec3 color) : color(color) {};
		operator glm::vec3() {
			return color;
		}
	};
}