#pragma once

#include "renderer/point_light.h"

namespace Engine {
	struct PointLightComponent {
		Renderer::PointLight light{};
	};
}