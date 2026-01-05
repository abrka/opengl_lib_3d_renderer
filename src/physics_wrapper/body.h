#pragma once

namespace JPH {
	class BodyInterface;
	class BodyCreationSettings;
	class Body;
}

namespace Physics {
	class World;

	class Body {
	public:
		JPH::Body* jph_body{};
		Body(World& physics_world, const JPH::BodyCreationSettings& jph_settings);
		Body(const Body&) = delete;
		Body(Body&& a) noexcept;
		Body& operator=(const Body&) = delete;
		Body& operator=(Body&& a) noexcept;
		~Body();
	private:
		JPH::BodyInterface& jph_body_interface;
	};
}