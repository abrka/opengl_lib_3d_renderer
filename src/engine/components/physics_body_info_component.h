#pragma once

#include <memory>
#include "physics_wrapper/world.h"
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace Engine {
	struct PhysicsBodyInfoComponent {
		std::unique_ptr<JPH::ShapeSettings> shape_settings{};
		JPH::EMotionType motion_type{};
		Physics::Layer layer{};
		bool requires_reload{true};

		PhysicsBodyInfoComponent() {
			shape_settings = std::make_unique<JPH::BoxShapeSettings>(JPH::RVec3{0.1f, 0.1f, 0.1f});
			shape_settings->SetEmbedded();
			motion_type = JPH::EMotionType::Dynamic;
			layer = Physics::Layer::MOVING;
		}
	};
}