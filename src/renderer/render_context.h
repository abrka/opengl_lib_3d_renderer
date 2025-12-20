#pragma once

#include <vector>
#include "camera.h"

namespace Renderer {
	struct DirectionalLight {
		glm::vec3 color{};
		float ambient_strength{};
	};
	struct RenderContext {
		Camera cam{};
		std::vector<DirectionalLight> directional_lights{};
	};
}