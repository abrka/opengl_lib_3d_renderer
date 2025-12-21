#pragma once

#include <glm/glm.hpp>

namespace Renderer {
	struct color_t {
		glm::vec3 color{};
		operator glm::vec3() {
			return color;
		}
	};
}