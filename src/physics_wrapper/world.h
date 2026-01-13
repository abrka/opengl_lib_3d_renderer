#pragma once

#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace JPH {
	class PhysicsSystem;
	class TempAllocatorImpl;
	class JobSystemThreadPool;
}
namespace Physics {
	enum class Layer : JPH::ObjectLayer
	{
		NON_MOVING = 0,
		MOVING = 1,
		NUM_LAYERS
	};

	class BPLayerInterfaceImpl;
	class ObjectVsBroadPhaseLayerFilterImpl;
	class ObjectLayerPairFilterImpl;

	class World {
	public:
		World();
		~World();
		void tick(float delta);
	public:
		std::unique_ptr<JPH::PhysicsSystem> jph_physics_system{};
	private:
		std::unique_ptr<JPH::TempAllocatorImpl> jph_temp_allocator{};
		std::unique_ptr<JPH::JobSystemThreadPool> jph_job_system{};
		std::unique_ptr<BPLayerInterfaceImpl> broad_phase_layer_interface{};
		std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> object_vs_broadphase_layer_filter{};
		std::unique_ptr<ObjectLayerPairFilterImpl> object_vs_object_layer_filter{};
	};
}