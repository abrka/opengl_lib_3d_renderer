#include "body.h"

#include "world.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Physics {
	Body::Body(World& physics_world, const JPH::BodyCreationSettings& jph_settings) : jph_body_interface(physics_world.jph_physics_system->GetBodyInterface())
	{
		jph_body = jph_body_interface.CreateBody(jph_settings);
		jph_body_interface.AddBody(jph_body->GetID(), JPH::EActivation::Activate);
	}
	Body::Body(Body&& a) noexcept : jph_body{a.jph_body} , jph_body_interface{a.jph_body_interface}
	{
		a.jph_body = nullptr;
	}
	Body& Body::operator=(Body&& a) noexcept
	{
		if (&a == this) {
			return *this;
		}

		jph_body = a.jph_body;
		a.jph_body = nullptr;

		return *this;
	}
	Body::~Body()
	{
		if (jph_body == nullptr) {
			return;
		}
		jph_body_interface.RemoveBody(jph_body->GetID());
		jph_body_interface.DestroyBody(jph_body->GetID());
	}
}