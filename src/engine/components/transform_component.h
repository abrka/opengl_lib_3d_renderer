#pragma once

#include <glm/glm.hpp>
#include <ImReflect.hpp>

namespace Engine {
	struct TransformComponent {
		glm::mat4 transform{};
	};
}
IMGUI_REFLECT(Engine::TransformComponent, transform)