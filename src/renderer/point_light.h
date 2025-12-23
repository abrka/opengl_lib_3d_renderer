#pragma once

#include <glm/glm.hpp>
#include "color_t.h"

namespace Renderer {
	struct PointLight {
		color_t color{};
		float ambient_strength{};
		float diffuse_strength{};
		float specular_strength{};
	};
}