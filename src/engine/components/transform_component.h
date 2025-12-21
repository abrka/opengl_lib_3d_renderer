#pragma once

#include <glm/glm.hpp>

namespace Engine {
	struct TransformComponent {
		glm::mat4 transform{ 1.0f };
	};
}