#pragma once

#include <glm/glm.hpp>

namespace Renderer {
	struct PointLight {
		glm::vec3 position{};
		glm::vec3 color{};
		float ambient_strength{};
	};
}