#pragma once

#include "color_t.h"

namespace Renderer {
	struct PointLight {
		color_t color{};
		float ambient_strength{};
		float diffuse_strength{};
		float specular_strength{};
		float constant{ 1.0f };
		float linear{ 0.09f };
		float quadratic{ 0.032f };
	};
}