#pragma once

#include <glm/glm.hpp>
#include "color_t.h"

namespace Renderer {
	struct PointLight {
		color_t color{glm::vec3(1.0f)};
		float ambient_strength{0.1f};
	};
}