#pragma once

#include <memory>
#include "physics_wrapper/body.h"

namespace Engine {
	struct PhysicsBodyComponent {
		std::unique_ptr<Physics::Body> body{};
	};
}