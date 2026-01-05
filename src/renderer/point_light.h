#pragma once

#include "color_t.h"

namespace Renderer {
	struct PointLight {
		color_t color{ glm::vec3{1.0f} };
		float ambient_strength{ 1.0f };
		float diffuse_strength{ 1.0f };
		float specular_strength{ 1.0f };
		float constant{ 1.0f };
		float linear{ 0.09f };
		float quadratic{ 0.032f };
	};
}