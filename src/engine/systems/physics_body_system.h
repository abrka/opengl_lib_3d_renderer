#pragma once

#include <entt/fwd.hpp>

namespace Physics {
	class World;
}

namespace Engine {
	void physics_body_load_system(entt::registry& entt_registry, Physics::World& physics_world);
	void physics_body_system_tick(entt::registry& entt_registry);
}